#include "system_configuration.h"
#include "usart.h"
#include "tim.h"
#include <string.h>
#include "handle_timer.h"
#include "ds1307rtc.h"
#include "uart_handler.h"

configuration_protocol_t protocol;

uint8_t pin_buffer[PIN_SIZE];
uint8_t delay1_buffer[DELAY_SIZE];
uint8_t delay2_buffer[DELAY_SIZE];
uint8_t duration_buffer[DURATION_SIZE];
uint8_t date_time_buffer[DATE_TIME_BUFFER_SIZE];

/**
 * Define variables that count the number of element inserted
 */
/*uint8_t pin_element_inserted = 0;
uint8_t delay1_element_inserted = 0;
uint8_t delay2_element_inserted = 0;
uint8_t duration_element_inserted = 0;
uint8_t date_time_element_inserted = 0;
uint8_t date_time_element_inserted = 0;*/

uint8_t slash = '/';
uint8_t space = ' ';
uint8_t colons = ':';
user_pin DEFAULT_USER_PIN[PIN_SIZE] = {'0','0','0','0'};

void init_protocol(system_configuration_t *configuration, TIM_HandleTypeDef *timer, rtc_t *rtc){

	protocol.configuration = configuration;

	protocol.rtc = rtc;

	protocol.state = IDLE;

	protocol.timer = timer;

	set_timer_period(protocol.timer, CONFIGURATION_PERIOD);

}

Protocol_Status_Type configuration_protocol(){

	protocol.state = START;

	send_message_IT((uint8_t *)SYSTEM_CONFIGURATION, strlen(SYSTEM_CONFIGURATION));

	start_timer_IT(protocol.timer);

	while(protocol.state != END_CUSTOM && protocol.state != END_DEFAULT){}

	if(protocol.state == END_DEFAULT){

		load_default_configuration();

		send_message_IT((uint8_t *)DEFAULT_CONFIGURATION_LOADED, strlen(DEFAULT_CONFIGURATION_LOADED));

	}else{

		if(check_configuration_fields() == PROTOCOL_OK){

			if(load_custom_configuration() == PROTOCOL_OK){

				send_message_IT((uint8_t *)CUSTOM_CONFIGURATION_LOADED, strlen(CUSTOM_CONFIGURATION_LOADED));

			}else{

				send_message_IT((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));

			}

		}else{

			load_default_configuration();

			send_message_IT((uint8_t *)DEFAULT_CONFIGURATION_LOADED, strlen(DEFAULT_CONFIGURATION_LOADED));

			protocol.state = END_DEFAULT;
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
