/*
 * handle_timer.h
 *
 *  Created on: May 24, 2020
 *      Author: 2017
 */

#ifndef INC_HANDLE_TIMER_H_
#define INC_HANDLE_TIMER_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "handle_timer.h"
#include "system_configuration.h"



/**
 * @brief initialize timer with given prescaler and period
 */
void init_timer(TIM_HandleTypeDef *timer, uint32_t prescaler, uint32_t period);


/**
 * @brief set timer's period.
 */
void set_timer_period(TIM_HandleTypeDef *timer, uint32_t period);

/**
 * @brief start timer in interrupt mode
 */
void start_timer_IT(TIM_HandleTypeDef *timer);

/**
 * @brief stop timer in interrupt mode
 */
void stop_timer_IT(TIM_HandleTypeDef *timer);


#endif /* INC_HANDLE_TIMER_H_ */
