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


void active_buzzer(buzzer *buzzer, buzzer_state state, int pulse, TIM_HandleTypeDef *timer){

	buzzer->state = BUZZER_ACTIVE;
	if(pulse == 299){
		buzzer->state = RINGTONE_PIR;
	}
	if(pulse == 499){
		buzzer->ringtone = RINGTONE_BARRIER;
	}
	if(pulse == 999){
		buzzer->ringtone = RINGTONE_BOTH;
	}
	__HAL_TIM_SET_COMPARE(timer, TIM_CHANNEL_2, pulse);
	HAL_TIM_PWM_Start_IT(timer, TIM_CHANNEL_2);

}


void deactive_buzzer(buzzer *buzzer, buzzer_state state, TIM_HandleTypeDef *timer){
	buzzer->state = BUZZER_INACTIVE;
	HAL_TIM_PWM_Stop_IT(timer, TIM_CHANNEL_2); //controllare qua
}
