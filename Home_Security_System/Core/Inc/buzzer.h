/*
 * buzzer.h
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "stm32f4xx_hal.h"

typedef enum{
	RINGTONE_PIR,
	RINGTONE_BARRIER,
	RINGTONE_BOTH
}ringtone;

typedef enum{
	BUZZER_ACTIVE,
	BUZZER_INACTIVE
}buzzer_state;

typedef struct{
	buzzer_state state;
	ringtone ringtone;
	TIM_HandleTypeDef *timer;
}buzzer;


/**
 * Initialize buzzer
 */
void buzzer_init(buzzer *buzzer, buzzer_state state, TIM_HandleTypeDef *timer);
/**
 * Basing on the input, it active buzzer with a specific ringtone
 */
void active_buzzer(buzzer *buzzer, buzzer_state state, ringtone ringtone, TIM_HandleTypeDef *timer);

/**
 * Deactivate buzzer
 */
void deactive_buzzer(buzzer *buzzer, buzzer_state state, TIM_HandleTypeDef *timer);

#endif /* INC_BUZZER_H_ */
