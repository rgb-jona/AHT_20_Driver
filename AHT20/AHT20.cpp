/*
 * AHT20.cpp
 *
 *  Created on: Jan 12, 2021
 *      Author: Jona Dallmann
 */

#include "AHT20.h"
#include <stm32f051x8.h>
#include <math.h>

AHT20::AHT20(I2C_HandleTypeDef *i2ch){
	_i2cHandler = i2ch;
}

// initialize function for sensor
void AHT20::init(){
	// wait for sensor startup
	HAL_Delay(50);
	_getStatus();
	// wait if Sensor is busy
	while(_sensorStatus & AHT20_STATUS_BUSY){
		HAL_Delay(40);
		_getStatus();
	}
	// check for calibration and if not initialize Sensor via command and check until
	// sensor signals calibration
	while(!(_sensorStatus & AHT20_STATUS_CALIBRATED)){
		uint8_t initializeData[3] = {AHT20_CMD_INITIALIZE, 0x08, 0x00};
		_sendData(initializeData, sizeof(initializeData));
		// wait suggested time after initialization and get sensor status
		HAL_Delay(15);
		_getStatus();
	}
}

// function to trigger measurement
float AHT20::measure(){
	// variables needed during the function
	uint8_t measurementResult[7] = {0,0,0,0,0,0,0};
	uint32_t temperatureBytes = 0;
	float temperature = 0;
	// send measurement trigger to sensor
	uint8_t outbuffer[3] = {AHT20_CMD_TRIGGER, 0x33, 0x00};
	_sendData(outbuffer, sizeof(outbuffer));
	// wait till measurement is completed
	HAL_Delay(85);
	// get sensor status, check and wait till measurement completed
	_getStatus();
	while(_sensorStatus & AHT20_STATUS_BUSY){
		HAL_Delay(85);
		_getStatus();
	}
	// read all result bytes from sensor
	HAL_I2C_Master_Receive(_i2cHandler, AHT20_I2CADDR_READ, measurementResult, sizeof(measurementResult), 10);
	//convert measurement by connecting bytes and converting result due to datasheet
	temperatureBytes = (measurementResult[3]&0x0F)<<16;
	temperatureBytes = temperatureBytes|(measurementResult[4]<<8);
	temperatureBytes = temperatureBytes|measurementResult[5];
	temperature = ((float)temperatureBytes/pow(2,20))*200-50;
	return temperature;
}

// wrapper to read and write data specific data of/from the sensor

// simple function to send data to the Sensor
HAL_StatusTypeDef AHT20::_sendData(uint8_t *data, int size){
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(_i2cHandler, AHT20_I2CADDR_WRITE, data, size, 10);
	return ret;
}

// simple reading function for reading sensor registers and writing them into a buffer
HAL_StatusTypeDef AHT20::_getStatus(){
	HAL_StatusTypeDef ret = HAL_I2C_Master_Receive(_i2cHandler, AHT20_I2CADDR_READ, &_sensorStatus, 1, 10);
	return ret;
}
