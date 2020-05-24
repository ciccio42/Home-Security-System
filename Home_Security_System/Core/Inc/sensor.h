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

typedef enum{
	ACTIVE,
	INACTIVE,
	ALARMED,
	DELAYED
}sensor_state;

typedef struct{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	sensor_state state;
	int pin_state_t;
}sensor;

/*typedef struct{
	sensor sensor;
	int threshold;
}analog_sensor;*/


#endif /* INC_SENSOR_H_ */
