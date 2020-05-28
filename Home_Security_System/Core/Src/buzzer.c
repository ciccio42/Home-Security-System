/*
 * buzzer.c
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */


#include "buzzer.h"
#include "stm32f4xx_hal.h"


void buzzer_init(buzzer *buzzer, buzzer_state state, TIM_HandleTypeDef *timer){

	buzzer->state = state;
	buzzer->timer =timer;
}


void active_buzzer(buzzer *buzzer, buzzer_state state, int period, TIM_HandleTypeDef *timer){

	buzzer->state = BUZZER_ACTIVE;
	if(period == 299){
		buzzer->state = RINGTONE_PIR;
	}
	if(period == 499){
		buzzer->ringtone = RINGTONE_BARRIER;
	}
	if(period == 999){
		buzzer->ringtone = RINGTONE_BOTH;
	}

	HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_2);
	__HAL_TIM_SET_AUTORELOAD(timer, period);
}


void deactive_buzzer(buzzer *buzzer, buzzer_state state, TIM_HandleTypeDef *timer){
	buzzer->state = BUZZER_INACTIVE;
	HAL_TIM_PWM_Stop_IT(timer, TIM_CHANNEL_2); //controllare qua
}
