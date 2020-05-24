/*
 * system_configuration.h
 *
 *  Created on: May 18, 2020
 *      Author: 2017
 */

#ifndef INC_SYSTEM_CONFIGURATION_H_
#define INC_SYSTEM_CONFIGURATION_H_

#include <stdint.h>
/**
 * @brief Define USER_PIN type
 */
typedef uint8_t *user_pin;
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
	user_pin pin;
	sensor_delay sensor_delay_1;
	sensor_delay sensor_delay_2;
	allarm_duration duration;
};

/**
 * @brief Define System configuration type
 */
typedef struct system_configuration_s system_configuration_t;

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
#define INSERT_DATE_TIME ("\n\rINSERT DATE and TIME [DD-MM-AAAA Hr:MM:SS]: ")
#define ERROR_STRING ("\n\rInvalid field inserted")

/**
 * @brief Define configuration's fields size
 */
#define PIN_SIZE (4)
#define DELAY_SIZE (2)
#define DURATION_SIZE (2)


/**
 * @brief Defines protocol status type
 */
typedef enum{
	START,
	PIN_T,
	PIN_R,
	DELAY1_T,
	DELAY1_R,
	DELAY2_T,
	DELAY2_R,
	DURATION_T,
	DURATION_R,
	DATE_TIME,
	END_DEFAULT,
	END_CUSTOM
} Protocol_Status_Type;

/**
 * @brief define protocol's data structure
 */
typedef struct{

	system_configuration_t configuration;
	Protocol_Status_Type state;

}configuration_protocol_t;

extern configuration_protocol_t protocol;

/**
 * @brief Initialize protocol's structure with the given system's configuration.
 */
void init_protocol(system_configuration_t *configuration);

/**
 * @brief Perform system_configuration procedure in a blocking way.
 *
 * @param system_configuration reference to a system configuration structure
 *
 * @return 1 if the procedure succeed, otherwise -1
 */
Protocol_Status_Type configuration_protocol();

/**
 * @brief load default configuration parameters.
 */
static void load_default_configuration();

/**
 * @brief load custom configuration parameters.
 */
static uint8_t load_custom_configuration();

/**
 * @brief whether the configuration parameters inserted by the user are valid or not.
 */
static uint8_t check_configuration_fields();

#endif /* INC_SYSTEM_CONFIGURATION_H_ */
