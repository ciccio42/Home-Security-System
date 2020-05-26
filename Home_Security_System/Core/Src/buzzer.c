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


void active_buzzer(buzzer *buzzer, buzzer_state state, ringtone ringtone, TIM_HandleTypeDef *timer){

	buzzer->state = BUZZER_ACTIVE;
	buzzer->ringtone = ringtone;
	if(ringtone == RINGTONE_PIR){
		__HAL_TIM_SET_AUTORELOAD(timer, 499);
	}
	if(ringtone == RINGTONE_BARRIER){
		__HAL_TIM_SET_AUTORELOAD(timer, 999); //questi timer sono htim3
	}
	if(ringtone == RINGTONE_BOTH){
		__HAL_TIM_SET_AUTORELOAD(timer, 1499);
	}

	HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_2);
}


void deactive_buzzer(buzzer *buzzer, buzzer_state state, TIM_HandleTypeDef *timer){
	buzzer->state = BUZZER_INACTIVE;
	HAL_TIM_PWM_Stop_IT(timer, TIM_CHANNEL_2); //controllare qua
}
