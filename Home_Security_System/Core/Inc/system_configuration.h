/*
 * system_configuration.h
 *
 *  Created on: May 18, 2020
 *      Author: 2017
 */

#ifndef INC_SYSTEM_CONFIGURATION_H_

#define INC_SYSTEM_CONFIGURATION_H_

#include <stdint.h>
#include "stm32f4xx.h"
#include "handle_timer.h"
#include "ds1307rtc.h"
#include "system.h"

/**
 * Define protocol Error
 */
#define PROTOCOL_OK (0)
#define PROTOCOL_ERR (-1)


/**
 * @brief Define default configuration values
 */
#define DEFAULT_DELAY (0)
#define DEFAULT_DURATION (5)

/**
 * @brief Define messages to sent through UART
 */
#define SYSTEM_CONFIGURATION ("SYSTEM BOOT ")
#define INSERT_PIN ("\n\rINSERT PIN [xxxx]: ")
#define INSERT_DELAY_SENSOR_1 ("\n\rISERT PIR's DELAY [xx]: ")
#define INSERT_DELAY_SENSOR_2 ("\n\rINSERT BARRIER's DELAY [xx]: ")
#define INSERT_ALLARM_DURATION ("\n\rINSERT ALLARM's DURATION [xx]: ")
#define INSERT_DATE_TIME ("\n\rINSERT DATE and TIME [dd/mm/yr hr:mm:ss]: ")

#define ERROR_STRING ("\n\rERROR RESTART THE BOARD")
#define CUSTOM_CONFIGURATION_LOADED ("\n\rCustom configuration loaded")
#define DEFAULT_CONFIGURATION_LOADED ("\n\rDefault configuration loaded")
/**
 * @brief Define configuration's fields size
 */
#define PIN_SIZE (4)
#define DELAY_SIZE (2)
#define DURATION_SIZE (2)
#define DATE_TIME_SIZE (2)
#define DATE_TIME_BUFFER_SIZE (12)

/**
 * @brief define timer's period for configuration task.
 */
#define CONFIGURATION_PERIOD (29999)

/**
 * @brief Defines protocol status type
 */
typedef enum{
	IDLE,
	START,
	PIN_T,
	PIN_R,
	DELAY1_T,
	DELAY1_R,
	DELAY2_T,
	DELAY2_R,
	DURATION_T,
	DURATION_R,
	DATE_T,
	DATE_R,
	MONTH_T,
	MONTH_R,
	YEAR_T,
	YEAR_R,
	HOUR_T,
	HOUR_R,
	MINUTE_T,
	MINUTE_R,
	SECOND_T,
	SECOND_R,
	END_DEFAULT,
	END_CUSTOM
} Protocol_Status_Type;

/**
 * @brief define protocol's data structure
 */
typedef struct{

	system_configuration_t *configuration;

	Protocol_Status_Type state;

	TIM_HandleTypeDef *timer;

	rtc_t *rtc;

}configuration_protocol_t;

extern configuration_protocol_t protocol;

extern  uint8_t pin_buffer[PIN_SIZE];
extern  uint8_t delay1_buffer[DELAY_SIZE];
extern  uint8_t delay2_buffer[DELAY_SIZE];
extern  uint8_t duration_buffer[DURATION_SIZE];
extern  uint8_t date_time_buffer[DATE_TIME_BUFFER_SIZE];

/**
 * Define variables that count the number of element inserted
 */
/*uint8_t pin_element_inserted = 0;
uint8_t delay1_element_inserted = 0;
uint8_t delay2_element_inserted = 0;
uint8_t duration_element_inserted = 0;
uint8_t date_time_element_inserted = 0;
uint8_t date_time_element_inserted = 0;*/

extern  uint8_t slash;
extern  uint8_t space;
extern  uint8_t colons;
extern  user_pin DEFAULT_USER_PIN[PIN_SIZE];

/**
 * @brief Initialize protocol's structure with the given system's configuration.
 */
void init_protocol(system_configuration_t *configuration, TIM_HandleTypeDef *timer, rtc_t *rtc);

/**
 * @brief Perform system_configuration procedure in a blocking way.
 */
Protocol_Status_Type configuration_protocol();

/**
 * @brief load default configuration parameters.
 */
static int8_t load_default_configuration();

/**
 * @brief load custom configuration parameters.
 */
static int8_t load_custom_configuration();

/**
 * @brief whether the configuration parameters inserted by the user are valid or not.
 */
static int8_t check_configuration_fields();

#endif /* INC_SYSTEM_CONFIGURATION_H_ */
