#include "system_configuration.h"
#include "usart.h"
#include "tim.h"
#include <string.h>

configuration_protocol_t protocol;

static uint8_t pin_buffer[PIN_SIZE];
static uint8_t delay1_buffer[DELAY_SIZE];
static uint8_t delay2_buffer[DELAY_SIZE];
static uint8_t duration_buffer[DURATION_SIZE];

uint8_t look = 0;

static const user_pin DEFAULT_USER_PIN[4] = {'0','0','0','0'};

Protocol_Status_Type configuration_protocol(){

	protocol.state = START;

	HAL_UART_Transmit_IT(&huart2, (uint8_t *)SYSTEM_CONFIGURATION, strlen(SYSTEM_CONFIGURATION));

	HAL_TIM_Base_Start_IT(&htim10);

	while(protocol.state != END_CUSTOM && protocol.state != END_DEFAULT){}

	if(protocol.state == END_DEFAULT){
		load_default_configuration();
	}else{
		if(check_configuration_fields()){

			load_custom_configuration();

		}else{

			HAL_UART_Transmit_IT(&huart2, (uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
			load_default_configuration();

		}
	}

	return protocol.state;

}


static uint8_t check_pin(uint8_t *pin){

	uint8_t i = 0;

	for(i=0; i<PIN_SIZE; i++){

		if(pin[i] < '0' || pin[i] > '9'){
			return -1;
		}

	}

	return 1;
}

static uint8_t check_delay(uint8_t *delay){

	uint8_t i = 0;

	for(i=0; i<DELAY_SIZE; i++){

		if((i == 0 && (delay[i] < '0' || delay[i] > '3'))
			|| (i == 1 && (delay[i] < '0' || (delay[i] > '9')))){

			return -1;

		}else if( (i == 1 && (delay[i] > '0' && delay[0] == '3' ))){

			return -1;

		}

	}

	return 1;

}

static uint8_t check_duration(uint8_t *duration){

	uint8_t i = 0;

	for(i=0; i<DURATION_SIZE; i++){

		if((i == 0 && (duration[i] < '0' || duration[i] > '6'))
			|| (i == 1 && (duration[i] < '0' || (duration[i] > '9')))){

			return -1;

		}else if( (i == 1 && (duration[i] > '0' && duration[0] == '6' ))){

			return -1;

		}

	}

	return 1;

}

static uint8_t check_configuration_fields(){

	if( check_pin(pin_buffer) != 1 || check_delay(delay1_buffer) != 1
		|| check_delay(delay2_buffer) != 1 || check_duration(duration_buffer) != 1){

		return -1;
	}

	return 1;
}

uint8_t convert_buffer_to_uint8(uint8_t *buffer){

	return (10*(buffer[0]-'0')) + (buffer[1] - '0');

}

static uint8_t load_custom_configuration(){

	protocol.configuration.pin = pin_buffer;

	protocol.configuration.sensor_delay_1 = convert_buffer_to_uint8(delay1_buffer);

	protocol.configuration.sensor_delay_2 = convert_buffer_to_uint8(delay2_buffer);

	protocol.configuration.duration =  convert_buffer_to_uint8(duration_buffer);

}

static void load_default_configuration(){

	protocol.configuration.pin = DEFAULT_USER_PIN;
	protocol.configuration.sensor_delay_1 = DEFAULT_DELAY;
	protocol.configuration.sensor_delay_2 = DEFAULT_DELAY;
	protocol.configuration.duration = DEFAULT_DURATION;

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	if(protocol.state == START){

		protocol.state = PIN_R; // wait for pin
		HAL_UART_Transmit_IT(&huart2,(uint8_t *)INSERT_PIN , strlen(INSERT_PIN));
		HAL_UART_Receive_IT(&huart2, pin_buffer, PIN_SIZE);

	}else if(protocol.state  == PIN_T){

		protocol.state = DELAY1_R; // wait for delay1
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)INSERT_DELAY_SENSOR_1,strlen(INSERT_DELAY_SENSOR_1));
		HAL_UART_Receive_IT(&huart2, delay1_buffer, DELAY_SIZE);

	}else if(protocol.state == DELAY1_T){

		protocol.state = DELAY2_R; // wait for delay2
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)INSERT_DELAY_SENSOR_2,strlen(INSERT_DELAY_SENSOR_2));
		HAL_UART_Receive_IT(&huart2, delay2_buffer, DELAY_SIZE);

	}else if(protocol.state == DELAY2_T){

		protocol.state = DURATION_R; // wait for duration
		HAL_UART_Transmit_IT(&huart2,(uint8_t *)INSERT_ALLARM_DURATION,strlen(INSERT_ALLARM_DURATION));
		HAL_UART_Receive_IT(&huart2, duration_buffer, DURATION_SIZE);

	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(protocol.state == PIN_R){

		protocol.state = PIN_T; //
		HAL_UART_Transmit_IT(&huart2,(uint8_t *)pin_buffer,PIN_SIZE);

	}else if(protocol.state == DELAY1_R){

		protocol.state = DELAY1_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)delay1_buffer,DELAY_SIZE);

	}else if(protocol.state == DELAY2_R){

		protocol.state = DELAY2_T;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)delay2_buffer,DELAY_SIZE);


	}else if(protocol.state == DURATION_R){

		HAL_TIM_Base_Stop(&htim10);
		protocol.state = END_CUSTOM;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)duration_buffer,DURATION_SIZE);

	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance == TIM10 && protocol.state != END_CUSTOM){

		HAL_UART_Abort_IT(&huart2);
		protocol.state = END_DEFAULT;

	}

	HAL_TIM_Base_Stop(&htim10);

}