/*
 * sensor.h
 *
 *  Created on: May 24, 2020
 *      Author: emiso
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

/**
 * Struct to define the sensor state
 */
typedef enum{
	SENSOR_ACTIVE,
	SENSOR_INACTIVE,
	SENSOR_ALARMED,
	SENSOR_DELAYED
}sensor_state;

/**
 * Struct to define the state, pin, port of a sensor
 */
typedef struct{
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	sensor_state state;
	int pin_state_t;
}sensor;

/**
 * Struct to extend struct sensor to implement analog sensor
 */
/*typedef struct{
	sensor sensor;
	int threshold;
}analog_sensor;*/


#endif /* INC_SENSOR_H_ */
