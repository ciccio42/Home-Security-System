/*
 * system.h
 *
 *  Created on: May 26, 2020
 *      Author: emiso
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_

#include "sensor_pir.h"
#include "sensor_barrier.h"
#include "buzzer.h"
#include "tim.h"
#include "uart_handler.h"
#include "ds1307rtc.h"

/**
 * @brief Define USER_PIN type
 */
typedef uint8_t user_pin;
/**
 * @brief Define Delay type;
 */
typedef uint8_t sensor_delay;

/**
 * @brief Define Duration type;
 */
typedef uint8_t allarm_duration;

/**
 * @brief Define Configuration structure
 */
struct system_configuration_s{

	user_pin *pin;
	sensor_delay sensor_delay_1;
	sensor_delay sensor_delay_2;
	allarm_duration duration;

};

/**
 * @brief Define System configuration type
 */
typedef struct system_configuration_s system_configuration_t;

typedef enum{
	SYSTEM_ACTIVE,
	SYSTEM_INACTIVE,
	SYSTEM_ALARMED,
}system_state;


typedef struct{

	system_state state;
	sensor_barrier *barrier;
	sensor_pir *pir;
	buzzer *buzzer;
	system_configuration_t *system_configuration;
	rtc_t *rtc;

} system_s;

/**
 * Initialize the system
 */
void system_init(system_s *system, system_state state, sensor_barrier *barrier, sensor_pir *pir, buzzer *buzzer);

/**
 * Active the system
 */
void active_system(system_s *system);

/**
 * Alarme the system
 */
void alarme_system(system_s *system);
/**
 * Deactive the system
 */
void deactive_system(system_s *system);

/**
 * Active the barrier sensor
 */
void active_sensor_barrier(system_s *system);

/**
 * Deactive the barrier sensor
 */
void deactive_sensor_barrier(system_s *system);

/**
 * Active the pir sensor
 */
void active_sensor_pir(system_s *system);

/**
 * Deactive the pir sensor
 */
void deactive_sensor_pir(system_s *system);

/**
 * Used from the pir callback to communicate its allarmed state
 */
void pir_allarmed(sensor_pir *pir);

/**
 * Used from the barrier callback to communicate its allarmed state
 */
void barrier_allarmed(sensor_barrier *barrier);

#endif /* INC_SYSTEM_H_ */
