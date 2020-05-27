/*
 * ds1307rtc.c
 *
 *  Created on: May 26, 2020
 *      Author: 2017
 */


#include "ds1307rtc.h"


/**
 * @brief definre BUFFER_SIZE
 */
#define BUFFER_SIZE (7)

/**
 * output buffer for write date and time into Device memory
 */
uint8_t input_buffer[BUFFER_SIZE];

/**
 * define months' days vector
 */
uint8_t months[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/**
 * @brief timer initializer
 * @param i2c, pointer to the i2c handler to use
 * @param date_time, pointer to the date and time structure to use
 * @param i2c, pointer to the I2C handler to use
 * @return the operation result
 */
int8_t ds1307rtc_init(rtc_t *rtc, I2C_HandleTypeDef *i2c){

	HAL_StatusTypeDef returnValue = HAL_I2C_IsDeviceReady(i2c, DS1307_ADDRESS, MAX_RETRY, HAL_MAX_DELAY);

	if(returnValue != HAL_OK)
	{
		return DS1307_ERR;

	}else{

		date_time_t date_time;

		rtc->date_time = date_time;

		rtc->i2c = i2c;

	}

	return DS1307_OK;

}

uint8_t convert_dec2BCD(uint8_t value){
	return (value/10<<4)+(value%10);

}

uint8_t convert_BCD2dec(uint8_t bcd_value){
	return 10*(bcd_value>>4 & 0x7) + (bcd_value & 0xF);
}

/**
 * @brief prepare the buffer for the writing operation
 * @param date_time, reference to data to be written
 * @param buffer, reference to output buffer
 */
static void prepare_out_buffer(date_time_t *date_time, uint8_t *buffer){

	buffer[0] = convert_dec2BCD(date_time->seconds);
	buffer[1] = convert_dec2BCD(date_time->minutes);
	buffer[2] = convert_dec2BCD(date_time->hours);
	buffer[3] = convert_dec2BCD(date_time->day);
	buffer[4] = convert_dec2BCD(date_time->date);
	buffer[5] = convert_dec2BCD(date_time->month);
	buffer[6] = convert_dec2BCD(date_time->year);
}

static void convert_input_buffer(date_time_t *date_time, uint8_t *buffer){

	date_time->seconds = convert_BCD2dec(buffer[0]);
	date_time->minutes = convert_BCD2dec(buffer[1]);
	date_time->hours = convert_BCD2dec(buffer[2]);
	date_time->day = convert_BCD2dec(buffer[3]);
	date_time->date = convert_BCD2dec(buffer[4]);
	date_time->month = convert_BCD2dec(buffer[5]);
	date_time->year = convert_BCD2dec(buffer[6]);

}

/**
 * @brief update the date_time structure referenced by the rtc pointer in no blocking mode
 * @param rtc, RTC handler whose date and time must be updated
 * @param operation result
 */
int8_t ds1307rtc_update_date_time_DMA(rtc_t *rtc){

	HAL_StatusTypeDef return_value= HAL_I2C_Mem_Read_DMA(rtc->i2c, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, input_buffer, BUFFER_SIZE);

	if(return_value != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;

}



/**
* @brief write the rtc's date_time value into memory, in non-blocking mode.
* @param rtc, pointer to the rtc handler to use
* @return operation result
*/
int8_t ds1307rtc_set_date_time_DMA(rtc_t *rtc){

	uint8_t out_buffer[BUFFER_SIZE];

	prepare_out_buffer(&(rtc->date_time), out_buffer);

	HAL_StatusTypeDef returnValue = HAL_I2C_Mem_Write_DMA(rtc->i2c, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, out_buffer, BUFFER_SIZE);

	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;
}

/**
 * @brief update the date_time structure referenced by the rtc pointer in blocking mode
 */
int8_t ds1307rtc_update_date_time(rtc_t *rtc){

	HAL_StatusTypeDef return_value;

	return_value = HAL_I2C_Mem_Read(rtc->i2c, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, input_buffer, BUFFER_SIZE,HAL_MAX_DELAY);

	if(return_value != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	convert_input_buffer(&(rtc->date_time), input_buffer);
	return DS1307_OK;

}

int8_t ds1307rtc_set_date_time(rtc_t *rtc){

	uint8_t out_buffer[BUFFER_SIZE];

	/*rtc->date_time.seconds = 30;
	rtc->date_time.minutes = 59;
	rtc->date_time.hours = 22;
	rtc->date_time.day = 7;
	rtc->date_time.date = 31;
	rtc->date_time.month = 5;
	rtc->date_time.year = 20;*/

	prepare_out_buffer(&(rtc->date_time), out_buffer);

	HAL_StatusTypeDef returnValue = HAL_I2C_Mem_Write(rtc->i2c, DS1307_ADDRESS, DS1307_SECONDS, ADDRESS_SIZE, out_buffer, BUFFER_SIZE,HAL_MAX_DELAY);

	if(returnValue != HAL_OK)
	{
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;

}

/**
 * @brief set the internal rtc date structure fields;
 * @return operation result
 */
int8_t set_date(rtc_t *rtc,int8_t year,int8_t month,int8_t date){

	if((year>=0 && year<=99) && (month>=1 && month<=12) && (date>=1 && date<=31)){

		if(date <= months[month-1]){

			rtc->date_time.year = year;
			rtc->date_time.month = month;
			rtc->date_time.date = date;
			return DS1307_OK;

		}

	}

	return DS1307_ERR;
}

/**
 * @brief set the internal rtc time structure fields;
 * @return operation result
 */
int8_t set_time(rtc_t *rtc,int8_t hour,int8_t minute,int8_t second){

	if((hour>=0 && hour<=23) && (minute>=0 && minute<=59) && (second>=0 && second<=59)){

		rtc->date_time.hours = hour;
		rtc->date_time.minutes = minute;
		rtc->date_time.seconds = second;

		return DS1307_OK;

	}

	return DS1307_ERR;
}

