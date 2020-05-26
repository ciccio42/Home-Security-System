/*
 * sensor_pir.c
 *
 *  Created on: May 24, 2020
 *      Author: emiso
 */

#include "sensor_pir.h"


void pir_init(sensor_pir *pir, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, sensor_state state, TIM_HandleTypeDef *timer){

	pir->sensor.state = state;
	pir->sensor.GPIO_Pin = GPIO_Pin;
	pir->sensor.GPIOx = GPIOx;
	pir->timer = timer;
}


sensor_state get_state(sensor_pir *pir){

	return pir->sensor.state;

}


void set_state(sensor_pir *pir, sensor_state state){

	pir->sensor.state = state;

}


