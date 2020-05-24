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
/*
 * @brief define Timer's prescaler
 */
#define TIMER_PRESCALER (15999)

/**
 * @brief define Timer's period for log task.
 */
#define LOG_PERIOD (9999)

/**
 * @brief define timer's period for configuration task.
 */
#define CONFIGURATION_PERIOD (29999)


/**
 * @brief initialize timer with given prescaler and period
 */
void init_timer(TIM_HandleTypeDef *timer, uint32_t prescaler, uint32_t period);

/**
 * @brief set timer's prescaler
 */
void set_timer_prescaler(TIM_HandleTypeDef *timer, uint32_t prescaler);


/**
 * @brief set timer's period.
 */
void set_timer_period(TIM_HandleTypeDef *timer, uint32_t period);



#endif /* INC_HANDLE_TIMER_H_ */
