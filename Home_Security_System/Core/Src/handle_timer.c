/*
 * handle_timer.c
 *
 *  Created on: May 24, 2020
 *      Author: 2017
 */


#include "handle_timer.h"
#include "system_configuration.h"
#include "usart.h"


void set_timer_period(TIM_HandleTypeDef *timer, uint32_t period){

	__HAL_TIM_SET_AUTORELOAD(timer, period); //set period

}

void start_timer_IT(TIM_HandleTypeDef *timer){

	HAL_TIM_Base_Start_IT(timer);

}

void stop_timer_IT(TIM_HandleTypeDef *timer){

	HAL_TIM_Base_Stop_IT(timer);

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance == TIM10 && protocol.state != END_CUSTOM){

		HAL_UART_Abort_IT(&huart2);
		protocol.state = END_DEFAULT;

	}

	stop_timer_IT(protocol.timer);

}

