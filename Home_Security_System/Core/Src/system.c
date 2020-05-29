/*
 * system.c
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */

#include "system.h"
#include "system_configuration.h"

void system_init(system_s *system, system_state state, sensor_barrier *barrier, sensor_pir *pir, buzzer *buzzer){

	system->state = state;
	system->barrier = barrier;
	system->pir = pir;
	system->buzzer = buzzer;
}

void active_system(system_s *system){

	system->state = SYSTEM_ACTIVE;
}


void alarme_system(system_s *system){

	system->state = SYSTEM_ALARMED;
}


void deactive_system(system_s *system){

	system->state = SYSTEM_INACTIVE;
}


void active_sensor_barrier(system_s *system){

}


void deactive_sensor_barrier(system_s *system){

}


void active_sensor_pir(system_s *system){

	if(system->state == SYSTEM_ACTIVE)
		system->pir->sensor.state = SENSOR_ACTIVE;
}


void deactive_sensor_pir(system_s *system){

	system->pir->sensor.state = SENSOR_INACTIVE;
}



void pir_allarmed(sensor_pir *pir){

}



void barrier_allarmed(sensor_barrier *barrier){

}
