/**
 * CPP library for the AHT20 Sensor created by Jona Dallmann
 */

#include <stm32f051x8.h>
#include "stm32f0xx_hal.h"

// defines the bytes for different commands/states
#define AHT20_I2CADDR_READ 0x71
#define AHT20_I2CADDR_WRITE 0x70
#define AHT20_CMD_TRIGGER 0xAC
#define AHT20_CMD_INITIALIZE 0xBE
#define AHT20_STATUS_BUSY 0x80
#define AHT20_STATUS_CALIBRATED 0x08

class AHT20
{
public:
  	AHT20(I2C_HandleTypeDef *i2ch);
	void init();
	float measure();

private:
	// declare i2c Handler so it is not needed to pass it to every function
	I2C_HandleTypeDef *_i2cHandler;

	uint8_t _sensorStatus;

	// basic wrappers for send and receive data from the sensor
	HAL_StatusTypeDef _sendData(uint8_t data[], int size);
	HAL_StatusTypeDef _getStatus();
};
