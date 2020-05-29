/*
 * uart_handler.c
 *
 *  Created on: May 28, 2020
 *      Author: 2017
 */


#include "uart_handler.h"
#include "system_configuration.h"
#include <stdint.h>
#include "string.h"

uart_handler_t uart_handler;

void uart_handler_init(UART_HandleTypeDef *huart){

	uart_handler.huart = huart;
}


int8_t send_message_IT(uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Transmit_IT(uart_handler.huart, buffer, buffer_size);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;
}

int8_t send_message(uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Transmit(uart_handler.huart, buffer, buffer_size, HAL_MAX_DELAY);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;

}

int8_t receive_message_IT(uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Receive_IT(uart_handler.huart, buffer, buffer_size);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;

}

int8_t receive_message(uint8_t *buffer, int16_t buffer_size){

	HAL_StatusTypeDef returnValue;

	returnValue = HAL_UART_Receive(uart_handler.huart, buffer, buffer_size,HAL_MAX_DELAY);

	if(returnValue == HAL_OK){
		return UART_OK;
	}

	return UART_ERR;

}


void protocol_callback_tx(){

	if(protocol.state == START){

			protocol.state = PIN_R;
			HAL_UART_Transmit_IT(uart_handler.huart,(uint8_t *)INSERT_PIN , strlen(INSERT_PIN));
			HAL_UART_Receive_IT(uart_handler.huart, (uint8_t*)pin_buffer, PIN_SIZE);// wait for receiving pin

		}else if(protocol.state  == PIN_T){

			protocol.state = DELAY1_R; // wait for delay1
			HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)INSERT_DELAY_SENSOR_1,strlen(INSERT_DELAY_SENSOR_1));
			HAL_UART_Receive_IT(uart_handler.huart, (uint8_t *)delay1_buffer, DELAY_SIZE); // wait for receiving the first delay

		}else if(protocol.state == DELAY1_T){

			protocol.state = DELAY2_R;
			HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)INSERT_DELAY_SENSOR_2,strlen(INSERT_DELAY_SENSOR_2));
			HAL_UART_Receive_IT(uart_handler.huart, (uint8_t *)delay2_buffer, DELAY_SIZE);// wait for receiving the second delay

		}else if(protocol.state == DELAY2_T){

			protocol.state = DURATION_R; // wait for duration
			HAL_UART_Transmit_IT(uart_handler.huart,(uint8_t *)INSERT_ALLARM_DURATION,strlen(INSERT_ALLARM_DURATION));
			HAL_UART_Receive_IT(uart_handler.huart, (uint8_t *)duration_buffer, DURATION_SIZE);// wait for receiving the duration

		}else if(protocol.state == DURATION_T){

			protocol.state = DATE_R;
			HAL_UART_Transmit_IT(uart_handler.huart,(uint8_t *)INSERT_DATE_TIME,strlen(INSERT_DATE_TIME));
			HAL_UART_Receive_IT(uart_handler.huart, date_time_buffer, DATE_TIME_SIZE);// wait for receiving the  date


		}else if(protocol.state == DATE_T){

			protocol.state = MONTH_R;
			HAL_UART_Transmit_IT(uart_handler.huart, &slash, 1);
			HAL_UART_Receive_IT(uart_handler.huart, date_time_buffer+DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving month

		}
		else if(protocol.state == MONTH_T){

			protocol.state = YEAR_R;
			HAL_UART_Transmit_IT(uart_handler.huart, &slash, 1);
			HAL_UART_Receive_IT(uart_handler.huart, date_time_buffer+2*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving year

		}
		else if(protocol.state == YEAR_T){

			protocol.state = HOUR_R;
			HAL_UART_Transmit_IT(uart_handler.huart, &space, 1);
			HAL_UART_Receive_IT(uart_handler.huart, date_time_buffer+3*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving hour

		}
		else if(protocol.state == HOUR_T){

			protocol.state = MINUTE_R;
			HAL_UART_Transmit_IT(uart_handler.huart, &colons,1);
			HAL_UART_Receive_IT(uart_handler.huart, date_time_buffer+4*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving minute

		}
		else if(protocol.state == MINUTE_T){

			protocol.state = SECOND_R;
			HAL_UART_Transmit_IT(uart_handler.huart,&colons,1);
			HAL_UART_Receive_IT(uart_handler.huart, date_time_buffer+5*DATE_TIME_SIZE, DATE_TIME_SIZE);// wait for receiving second

		}
		else if(protocol.state == SECOND_T){

			stop_timer_IT(protocol.timer);
			protocol.state = END_CUSTOM;

		}

}

void protocol_callback_rx(){

	if(protocol.state == PIN_R ){

		protocol.state = PIN_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)pin_buffer, PIN_SIZE);

	}else if(protocol.state == DELAY1_R){

		protocol.state = DELAY1_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)delay1_buffer,DELAY_SIZE);

	}else if(protocol.state == DELAY2_R){

		protocol.state = DELAY2_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)delay2_buffer,DELAY_SIZE);

	}else if(protocol.state == DURATION_R){

		protocol.state = DURATION_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)duration_buffer,DURATION_SIZE);

	}
	else if(protocol.state == DATE_R){

		protocol.state = DATE_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)date_time_buffer,DATE_TIME_SIZE);

	}
	else if(protocol.state == MONTH_R){

		protocol.state = MONTH_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)date_time_buffer+DATE_TIME_SIZE,DATE_TIME_SIZE);

	}
	else if(protocol.state == YEAR_R){

		protocol.state = YEAR_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)date_time_buffer+2*DATE_TIME_SIZE,DATE_TIME_SIZE);

	}
	else if(protocol.state == HOUR_R){

		protocol.state = HOUR_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)date_time_buffer+3*DATE_TIME_SIZE,DATE_TIME_SIZE);

	}
	else if(protocol.state == MINUTE_R){

		protocol.state = MINUTE_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)date_time_buffer+4*DATE_TIME_SIZE,DATE_TIME_SIZE);

	}
	else if(protocol.state == SECOND_R){

		protocol.state = SECOND_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)date_time_buffer+5*DATE_TIME_SIZE,DATE_TIME_SIZE);

	}


}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	protocol_callback_tx();

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	protocol_callback_rx();

}
