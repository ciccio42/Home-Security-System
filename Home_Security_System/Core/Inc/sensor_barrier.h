/*
 * sensor_barrier.h
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */

#ifndef INC_SENSOR_BARRIER_H_
#define INC_SENSOR_BARRIER_H_

typedef struct{
	sensor sensor;
	TIM_HandleTypeDef *timer;
}sensor_barrier;

#endif /* INC_SENSOR_BARRIER_H_ */
