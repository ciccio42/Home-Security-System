/*
 * uart_handler.h
 *
 *  Created on: May 28, 2020
 *      Author: 2017
 */

#ifndef INC_UART_HANDLER_H_
#define INC_UART_HANDLER_H_


#include "stm32f4xx.h"

#define UART_OK (0)
#define UART_ERR (1)

struct uart_handler_s{

	UART_HandleTypeDef *huart;

};

typedef struct uart_handler_s uart_handler_t;

void uart_handler_init(UART_HandleTypeDef *huart);

int8_t send_message_IT(uint8_t *buffer, int16_t buffer_size);

int8_t send_message(uint8_t *buffer, int16_t buffer_size);

int8_t receive_message_IT(uint8_t *buffer, int16_t buffer_size);

int8_t receive_message(uint8_t *buffer, int16_t buffer_size);

#endif /* INC_UART_HANDLER_H_ */
