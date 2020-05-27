#include "system_configuration.h"
#include "usart.h"
#include "tim.h"
#include <string.h>
#include "handle_timer.h"
#include "ds1307rtc.h"

configuration_protocol_t protocol;

static uint8_t pin_buffer[PIN_SIZE];
static uint8_t delay1_buffer[DELAY_SIZE];
static uint8_t delay2_buffer[DELAY_SIZE];
static uint8_t duration_buffer[DURATION_SIZE];
static uint8_t date_time_buffer[DATE_TIME_BUFFER_SIZE];

/**
 * Define variables that count the number of element inserted
 */
/*uint8_t pin_element_inserted = 0;
uint8_t delay1_element_inserted = 0;
uint8_t delay2_element_inserted = 0;
uint8_t duration_element_inserted = 0;
uint8_t date_time_element_inserted = 0;
uint8_t date_time_element_inserted = 0;*/

const uint8_t slash = '/';
const uint8_t space = ' ';
const uint8_t colons = ':';
const user_pin DEFAULT_USER_PIN[PIN_SIZE] = {'0','0','0','0'};

void init_protocol(system_configuration_t *configuration, TIM_HandleTypeDef *timer, rtc_t *rtc){

	protocol.configuration = configuration;

	protocol.rtc = rtc;

	protocol.state = IDLE;

	protocol.timer = timer;

	set_timer_period(protocol.timer, CONFIGURATION_PERIOD);

}

Protocol_Status_Type configuration_protocol(){

	protocol.state = START;

	HAL_UART_Transmit_IT(&huart2, (uint8_t *)SYSTEM_CONFIGURATION, strlen(SYSTEM_CONFIGURATION));

	start_timer_IT(protocol.timer);

	while(protocol.state != END_CUSTOM && protocol.state != END_DEFAULT){}

	if(protocol.state == END_DEFAULT){

		load_default_configuration();
		HAL_UART_Transmit(&huart2,(uint8_t *)DEFAULT_CONFIGURATION_LOADED, strlen(DEFAULT_CONFIGURATION_LOADED), HAL_MAX_DELAY);

	}else{

		if(check_configuration_fields() == PROTOCOL_OK){

			if(load_custom_configuration() == PROTOCOL_OK){
				HAL_UART_Transmit_IT(&huart2,(uint8_t *)CUSTOM_CONFIGURATION_LOADED, strlen(CUSTOM_CONFIGURATION_LOADED));
			}else{
				HAL_UART_Transmit_IT(&huart2,(uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
			}


		}else{

			load_default_configuration();
			HAL_UART_Transmit(&huart2,(uint8_t *)DEFAULT_CONFIGURATION_LOADED, strlen(DEFAULT_CONFIGURATION_LOADED), HAL_MAX_DELAY);

		}
	}

	return protocol.state;

}

uint8_t convert_buffer_to_uint8(uint8_t *buffer){

	return (10*(buffer[0]-'0')) + (buffer[1] - '0');

}

static int8_t check_pin(uint8_t *pin){

	uint8_t i = 0;

	for(i=0; i<PIN_SIZE; i++){

		if(pin[i] < '0' || pin[i] > '9'){
			return PROTOCOL_ERR;
		}

	}

	return PROTOCOL_OK;
}

static int8_t check_delay(uint8_t *delay){

	uint8_t i = 0;

	for(i=0; i<DELAY_SIZE; i++){

		if((i == 0 && (delay[i] < '0' || delay[i] > '3'))
			|| (i == 1 && (delay[i] < '0' || (delay[i] > '9')))){

			return PROTOCOL_ERR;

		}else if( (i == 1 && (delay[i] > '0' && delay[0] == '3' ))){

			return PROTOCOL_ERR;

		}

	}

	return PROTOCOL_OK;

}

static int8_t check_duration(uint8_t *duration){

	uint8_t i = 0;

	for(i=0; i<DURATION_SIZE; i++){

		if((i == 0 && (duration[i] < '0' || duration[i] > '6'))
			|| (i == 1 && (duration[i] < '0' || (duration[i] > '9')))){

			return PROTOCOL_ERR;

		}else if( (i == 1 && (duration[i] > '0' && duration[0] == '6' ))){

			return PROTOCOL_ERR;

		}

	}

	return PROTOCOL_OK;

}


static int8_t take_date_time_from_buffer(uint8_t *date_time){

	int8_t date = convert_buffer_to_uint8(date_time);
	int8_t month = convert_buffer_to_uint8(date_time+DATE_TIME_SIZE);
	int8_t year = convert_buffer_to_uint8(date_time+2*DATE_TIME_SIZE);
	int8_t hour = convert_buffer_to_uint8(date_time+3*DATE_TIME_SIZE);
	int8_t minute = convert_buffer_to_uint8(date_time+4*DATE_TIME_SIZE);
	int8_t second = convert_buffer_to_uint8(date_time+5*DATE_TIME_SIZE);

	if(set_date(protocol.rtc,year,month,date) == DS1307_OK && set_time(protocol.rtc, hour, minute, second) == DS1307_OK){
		return PROTOCOL_OK;
	}

	return PROTOCOL_ERR;
}


static int8_t check_date_time(uint8_t *date_time){

	int i=0;

	for(i=0; i<DATE_TIME_BUFFER_SIZE; i++){

		if(date_time[i]<'0' || date_time[i] > '9'){
			return PROTOCOL_ERR;
		}

	}

	return take_date_time_from_buffer(date_time);
}

static int8_t check_configuration_fields(){

	if( check_pin(pin_buffer) != PROTOCOL_OK || check_delay(delay1_buffer) != PROTOCOL_OK
		|| check_delay(delay2_buffer) != PROTOCOL_OK || check_duration(duration_buffer) != PROTOCOL_OK || check_date_time(date_time_buffer) != PROTOCOL_OK){

		return PROTOCOL_ERR;
	}

	return PROTOCOL_OK;
}


static int8_t load_custom_configuration(){

	protocol.configuration->pin = pin_buffer;

	protocol.configuration->sensor_delay_1 = convert_buffer_to_uint8(delay1_buffer);

	protocol.configuration->sensor_delay_2 = convert_buffer_to_uint8(delay2_buffer);

	protocol.configuration->duration =  convert_buffer_to_uint8(duration_buffer);

	if (ds1307rtc_set_date_time(protocol.rtc) == DS1307_OK){
		return PROTOCOL_OK;
	}else{
		return PROTOCOL_ERR;
	}
}

static int8_t load_default_configuration(){

	protocol.configuration->pin = &DEFAULT_USER_PIN;
	protocol.configuration->sensor_delay_1 = DEFAULT_DELAY;
	protocol.configuration->sensor_delay_2 = DEFAULT_DELAY;
	protocol.configuration->duration = DEFAULT_DURATION;

	if (ds1307rtc_update_date_time(protocol.rtc) == DS1307_OK){
		return PROTOCOL_OK;
	}else{
		return PROTOCOL_ERR;
	}

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	if(protocol.state == START){

		protocol.state = PIN_R;
		HAL_UART_Transmit_IT(&huart2,(uint8_t *)INSERT_PIN , strlen(INSERT_PIN));
		HAL_UART_Receive_IT(&huart2, (uint8_t*)pin_buffer, PIN_SIZE);// wait for receiving pin
		//HAL_UART_Receive_IT(&huart2, &pin_buffer[pin_element_inserted], 1); // wait for receiving the first pin byte

	}else if(protocol.state  == PIN_T){

		protocol.state = DELAY1_R; // wait for delay1
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)INSERT_DELAY_SENSOR_1,strlen(INSERT_DELAY_SENSOR_1));
		HAL_UART_Receive_IT(&huart2, (uint8_t *)delay1_buffer, DELAY_SIZE); // wait for receiving the first delay
		/*if(pin_element_inserted < PIN_SIZE){ // check if the user has inserted all the 4 bytes for the pin

			protocol.state = PIN_R;
			HAL_UART_Receive_DMA(&huart2, &pin_buffer[pin_element_inserted], 1); // wait for receiving the next byte.

		}else{

			protocol.state = DELAY1_R; // wait for delay1
			HAL_UART_Transmit_IT(&huart2, (uint8_t *)INSERT_DELAY_SENSOR_1,strlen(INSERT_DELAY_SENSOR_1));
			HAL_UART_Receive_DMA(&huart2, &delay1_buffer[delay1_element_inserted], 1); // wait for receiving the first byte

		}*/

	}else if(protocol.state == DELAY1_T){

		protocol.state = DELAY2_R;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)INSERT_DELAY_SENSOR_2,strlen(INSERT_DELAY_SENSOR_2));
		HAL_UART_Receive_IT(&huart2, (uint8_t *)delay2_buffer, DELAY_SIZE);// wait for receiving the second delay
		/*if(delay1_element_inserted < DELAY_SIZE){// check if the user has inserted all the 2 bytes for the delay

			protocol.state = DELAY1_R;
			HAL_UART_Receive_DMA(&huart2, &delay1_buffer[delay1_element_inserted], 1); // wait for receiving the next byte

		}else{

			protocol.state = DELAY2_R;
			HAL_UART_Transmit_IT(&huart2, (uint8_t *)INSERT_DELAY_SENSOR_2,strlen(INSERT_DELAY_SENSOR_2));
			HAL_UART_Receive_DMA(&huart2, &delay2_buffer[delay2_element_inserted], 1);// wait for receiving the first byte

		}*/

	}else if(protocol.state == DELAY2_T){

		protocol.state = DURATION_R; // wait for duration
		HAL_UART_Transmit_IT(&huart2,(uint8_t *)INSERT_ALLARM_DURATION,strlen(INSERT_ALLARM_DURATION));
		HAL_UART_Receive_IT(&huart2, (uint8_t *)duration_buffer, DURATION_SIZE);// wait for receiving the duration

		/*if(delay2_element_inserted < DELAY_SIZE){// check if the user has inserted all the 2 bytes for the delay

			protocol.state = DELAY2_R;
			HAL_UART_Receive_IT(&huart2, &delay2_buffer[delay2_element_inserted], DELAY_SIZE);

		}else{

			protocol.state = DURATION_R; // wait for duration
			HAL_UART_Transmit_IT(&huart2,(uint8_t *)INSERT_ALLARM_DURATION,strlen(INSERT_ALLARM_DURATION));
			HAL_UART_Receive_DMA(&huart2, &duration_buffer[duration_element_inserted], 1);// wait for receiving the first duration byte

		}*/

	}else if(protocol.state == DURATION_T){

		protocol.state = DATE_R;
		HAL_UART_Transmit_IT(&huart2,(uint8_t *)INSERT_DATE_TIME,strlen(INSERT_DATE_TIME));
		HAL_UART_Receive_IT(&huart2, date_time_buffer, DATE_TIME_SIZE);// wait for receiving the  date
		/*if(duration_element_inserted < DURATION_SIZE){

			protocol.state = DURATION_R;
			HAL_UART_Receive_DMA(&huart2, &duration_buffer[duration_element_inserted], 1);// wait for receiving the next duration byte

		}else{

			protocol.state = DATE_TIME_R;
			HAL_UART_Transmit_IT(&huart2,(uint8_t *)INSERT_DATE_TIME,strlen(INSERT_DATE_TIME));
			HAL_UART_Receive_DMA(&huart2, &date_time_buffer[date_time_element_inserted], 1);// wait for receiving the first date_time byte
		}*/

	}/*else if(protocol.state == DATE_TIME_T){




		if(date_time_element_inserted < DATE_TIME_SIZE){

			protocol.state = DATE_TIME_R;
			HAL_UART_Receive_IT(&huart2, &date_time_buffer[date_time_element_inserted], 1); // wait for receiving the next date_time byte

		}else{


			protocol.state = END_CUSTOM;
			stop_timer_IT(protocol.timer);

		}*/
	else if(protocol.state == DATE_T){

		protocol.state = MONTH_R;
		HAL_UART_Transmit_IT(&huart2, &slash, 1);
		HAL_UART_Receive_IT(&huart2, date_time_buffer+DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving month

	}
	else if(protocol.state == MONTH_T){

		protocol.state = YEAR_R;
		HAL_UART_Transmit_IT(&huart2, &slash, 1);
		HAL_UART_Receive_IT(&huart2, date_time_buffer+2*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving year

	}
	else if(protocol.state == YEAR_T){

		protocol.state = HOUR_R;
		HAL_UART_Transmit_IT(&huart2, &space, 1);
		HAL_UART_Receive_IT(&huart2, date_time_buffer+3*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving hour

	}
	else if(protocol.state == HOUR_T){

		protocol.state = MINUTE_R;
		HAL_UART_Transmit_IT(&huart2, &colons,1);
		HAL_UART_Receive_IT(&huart2, date_time_buffer+4*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving minute

	}
	else if(protocol.state == MINUTE_T){

		protocol.state = SECOND_R;
		HAL_UART_Transmit_IT(&huart2,&colons,1);
		HAL_UART_Receive_IT(&huart2, date_time_buffer+5*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving second

	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(protocol.state == PIN_R ){

		protocol.state = PIN_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)pin_buffer, PIN_SIZE);
		/*pin_element_inserted += 1;
		HAL_UART_Transmit_IT(&huart2, &pin_buffer[pin_element_inserted-1], 1);*/

	}else if(protocol.state == DELAY1_R){

		protocol.state = DELAY1_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)delay1_buffer,DELAY_SIZE);
		/*delay1_element_inserted += 1;
		HAL_UART_Transmit_IT(&huart2, &delay1_buffer[delay1_element_inserted-1],1);*/

	}else if(protocol.state == DELAY2_R){

		protocol.state = DELAY2_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)delay2_buffer,DELAY_SIZE);
		/*delay2_element_inserted += 1;
		HAL_UART_Transmit_IT(&huart2, &delay2_buffer[delay2_element_inserted-1],1);*/


	}else if(protocol.state == DURATION_R){

		protocol.state = DURATION_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)duration_buffer,DURATION_SIZE);
		/*duration_element_inserted += 1;
		HAL_UART_Transmit_IT(&huart2, &duration_buffer[duration_element_inserted-1],1);*/

	}
	else if(protocol.state == DATE_R){

		protocol.state = DATE_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)date_time_buffer,DATE_TIME_SIZE);

	}
	else if(protocol.state == MONTH_R){

		protocol.state = MONTH_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)date_time_buffer+DATE_TIME_SIZE,DATE_TIME_SIZE);

	}
	else if(protocol.state == YEAR_R){

		protocol.state = YEAR_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)date_time_buffer+2*DATE_TIME_SIZE,DATE_TIME_SIZE);

	}
	else if(protocol.state == HOUR_R){

		protocol.state = HOUR_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)date_time_buffer+3*DATE_TIME_SIZE,DATE_TIME_SIZE);

	}
	else if(protocol.state == MINUTE_R){

		protocol.state = MINUTE_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)date_time_buffer+4*DATE_TIME_SIZE,DATE_TIME_SIZE);

	}
	else if(protocol.state == SECOND_R){

		stop_timer_IT(protocol.timer);
		protocol.state = END_CUSTOM;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)date_time_buffer+5*DATE_TIME_SIZE,DATE_TIME_SIZE);

	}

	//HAL_UART_Transmit_IT(&huart2, (uint8_t *)date_time_buffer,DATE_TIME_SIZE);
	/*protocol.state = DATE_TIME_T;
}				date_time_element_inserted +=1;
		HAL_UART_Transmit_IT(&huart2, &date_time_buffer[date_time_element_inserted-1],DATE_TIME_SIZE);*/
}
