/*
 * sensor_pir.h
 *
 *  Created on: May 24, 2020
 *      Author: emiso
 */

#ifndef INC_SENSOR_PIR_H_
#define INC_SENSOR_PIR_H_
#import "sensor.h"

typedef struct{
	sensor sensor;
}sensor_pir;


void pir_init(sensor_pir* pir, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, sensor_state state);

sensor_state get_state(sensor_pir* pir);

void set_state(sensor_pir* pir, sensor_state state);


#endif /* INC_SENSOR_PIR_H_ */
