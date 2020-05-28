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
			//HAL_UART_Receive_IT(uart_handler.huart, &pin_buffer[pin_element_inserted], 1); // wait for receiving the first pin byte

		}else if(protocol.state  == PIN_T){

			protocol.state = DELAY1_R; // wait for delay1
			HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)INSERT_DELAY_SENSOR_1,strlen(INSERT_DELAY_SENSOR_1));
			HAL_UART_Receive_IT(uart_handler.huart, (uint8_t *)delay1_buffer, DELAY_SIZE); // wait for receiving the first delay
			/*if(pin_element_inserted < PIN_SIZE){ // check if the user has inserted all the 4 bytes for the pin

				protocol.state = PIN_R;
				HAL_UART_Receive_DMA(uart_handler.huart, &pin_buffer[pin_element_inserted], 1); // wait for receiving the next byte.

			}else{

				protocol.state = DELAY1_R; // wait for delay1
				HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)INSERT_DELAY_SENSOR_1,strlen(INSERT_DELAY_SENSOR_1));
				HAL_UART_Receive_DMA(uart_handler.huart, &delay1_buffer[delay1_element_inserted], 1); // wait for receiving the first byte

			}*/

		}else if(protocol.state == DELAY1_T){

			protocol.state = DELAY2_R;
			HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)INSERT_DELAY_SENSOR_2,strlen(INSERT_DELAY_SENSOR_2));
			HAL_UART_Receive_IT(uart_handler.huart, (uint8_t *)delay2_buffer, DELAY_SIZE);// wait for receiving the second delay
			/*if(delay1_element_inserted < DELAY_SIZE){// check if the user has inserted all the 2 bytes for the delay

				protocol.state = DELAY1_R;
				HAL_UART_Receive_DMA(uart_handler.huart, &delay1_buffer[delay1_element_inserted], 1); // wait for receiving the next byte

			}else{

				protocol.state = DELAY2_R;
				HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)INSERT_DELAY_SENSOR_2,strlen(INSERT_DELAY_SENSOR_2));
				HAL_UART_Receive_DMA(uart_handler.huart, &delay2_buffer[delay2_element_inserted], 1);// wait for receiving the first byte

			}*/

		}else if(protocol.state == DELAY2_T){

			protocol.state = DURATION_R; // wait for duration
			HAL_UART_Transmit_IT(uart_handler.huart,(uint8_t *)INSERT_ALLARM_DURATION,strlen(INSERT_ALLARM_DURATION));
			HAL_UART_Receive_IT(uart_handler.huart, (uint8_t *)duration_buffer, DURATION_SIZE);// wait for receiving the duration

			/*if(delay2_element_inserted < DELAY_SIZE){// check if the user has inserted all the 2 bytes for the delay

				protocol.state = DELAY2_R;
				HAL_UART_Receive_IT(uart_handler.huart, &delay2_buffer[delay2_element_inserted], DELAY_SIZE);

			}else{

				protocol.state = DURATION_R; // wait for duration
				HAL_UART_Transmit_IT(uart_handler.huart,(uint8_t *)INSERT_ALLARM_DURATION,strlen(INSERT_ALLARM_DURATION));
				HAL_UART_Receive_DMA(uart_handler.huart, &duration_buffer[duration_element_inserted], 1);// wait for receiving the first duration byte

			}*/

		}else if(protocol.state == DURATION_T){

			protocol.state = DATE_R;
			HAL_UART_Transmit_IT(uart_handler.huart,(uint8_t *)INSERT_DATE_TIME,strlen(INSERT_DATE_TIME));
			HAL_UART_Receive_IT(uart_handler.huart, date_time_buffer, DATE_TIME_SIZE);// wait for receiving the  date
			/*if(duration_element_inserted < DURATION_SIZE){

				protocol.state = DURATION_R;
				HAL_UART_Receive_DMA(uart_handler.huart, &duration_buffer[duration_element_inserted], 1);// wait for receiving the next duration byte

			}else{

				protocol.state = DATE_TIME_R;
				HAL_UART_Transmit_IT(uart_handler.huart,(uint8_t *)INSERT_DATE_TIME,strlen(INSERT_DATE_TIME));
				HAL_UART_Receive_DMA(uart_handler.huart, &date_time_buffer[date_time_element_inserted], 1);// wait for receiving the first date_time byte
			}*/

		}/*else if(protocol.state == DATE_TIME_T){




			if(date_time_element_inserted < DATE_TIME_SIZE){

				protocol.state = DATE_TIME_R;
				HAL_UART_Receive_IT(uart_handler.huart, &date_time_buffer[date_time_element_inserted], 1); // wait for receiving the next date_time byte

			}else{


				protocol.state = END_CUSTOM;
				stop_timer_IT(protocol.timer);

			}*/
		else if(protocol.state == DATE_T){

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
		/*pin_element_inserted += 1;
		HAL_UART_Transmit_IT(uart_handler.huart, &pin_buffer[pin_element_inserted-1], 1);*/

	}else if(protocol.state == DELAY1_R){

		protocol.state = DELAY1_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)delay1_buffer,DELAY_SIZE);
		/*delay1_element_inserted += 1;
		HAL_UART_Transmit_IT(uart_handler.huart, &delay1_buffer[delay1_element_inserted-1],1);*/

	}else if(protocol.state == DELAY2_R){

		protocol.state = DELAY2_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)delay2_buffer,DELAY_SIZE);
		/*delay2_element_inserted += 1;
		HAL_UART_Transmit_IT(uart_handler.huart, &delay2_buffer[delay2_element_inserted-1],1);*/


	}else if(protocol.state == DURATION_R){

		protocol.state = DURATION_T;
		HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)duration_buffer,DURATION_SIZE);
		/*duration_element_inserted += 1;
		HAL_UART_Transmit_IT(uart_handler.huart, &duration_buffer[duration_element_inserted-1],1);*/

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

	//HAL_UART_Transmit_IT(uart_handler.huart, (uint8_t *)date_time_buffer,DATE_TIME_SIZE);
	/*protocol.state = DATE_TIME_T;
}				date_time_element_inserted +=1;
		HAL_UART_Transmit_IT(uart_handler.huart, &date_time_buffer[date_time_element_inserted-1],DATE_TIME_SIZE);*/

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	protocol_callback_tx();

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	protocol_callback_rx();

}
