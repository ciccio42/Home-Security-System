/*
 * sensor_pir.h
 *
 *  Created on: May 24, 2020
 *      Author: emiso
 */

#ifndef INC_SENSOR_PIR_H_
#define INC_SENSOR_PIR_H_

#include "sensor.h"
#include "stm32f4xx_hal.h"

#define STABLE_SIGNAL_PERIOD (999)

typedef struct{
	sensor sensor;
	TIM_HandleTypeDef *timer;
}sensor_pir;


/**
 * To initialize the pir sensor
 * It sets:
 *  - State
 *  - Pin
 *  - Port
 *  - Timer
 */
void pir_init(sensor_pir *pir, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, sensor_state state, TIM_HandleTypeDef *timer);

/**
 * To get the state of sensor pir
 */
sensor_state get_state(sensor_pir *pir);

/**
 * To set the state of sensor pir
 */
void set_state(sensor_pir *pir, sensor_state state);


#endif /* INC_SENSOR_PIR_H_ */
