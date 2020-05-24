/*
 * handle_timer.c
 *
 *  Created on: May 24, 2020
 *      Author: 2017
 */


#include "handle_timer.h"



void init_timer(TIM_HandleTypeDef *timer, uint32_t prescaler, uint32_t period){

	timer->Init.Prescaler = prescaler;
	timer->Init.Period = period;

}


void set_timer_prescaler(TIM_HandleTypeDef *timer, uint32_t prescaler){

	timer->Init.Prescaler = prescaler;

}


void set_timer_period(TIM_HandleTypeDef *timer, uint32_t period){

	timer->Init.Period = period;

}
