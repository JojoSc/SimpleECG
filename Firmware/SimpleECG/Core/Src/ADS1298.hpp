/*
 * ADS1298.h
 *
 *  Created on: Apr 2, 2023
 *      Author: Johannes S.
 */

#ifndef SRC_ADS1298_HPP_
#define SRC_ADS1298_HPP_

#include "stm32f1xx.h"


#include "PinOutput.hpp"
#include "main.h"

// ADS1299 commands
#define CMD_NOP     0x00 // Do nothing
#define CMD_WAKEUP  0x02 // Wake up from standby mode
#define CMD_STANDBY 0x04 // Enter standby mode
#define CMD_RESET   0x06 // Reset the device
#define CMD_START   0x08 // Start and restart conversions
#define CMD_STOP    0x0A // Stop conversion
#define CMD_RDATAC  0x10 // Enable 'read data continuously' mode (default at power up)
#define CMD_SDATAC  0x11 // Stop continuous mode
#define CMD_RDATA   0x12 // Read data (once)
#define CMD_RREG    0x20 // Read register
#define CMD_WREG    0x40 // Write register

// Register addresses
#define REG_ADDR_ID         0x00
#define REG_ADDR_CONFIG1    0x01
#define REG_ADDR_CONFIG2    0x02
#define REG_ADDR_CONFIG3    0x03
#define REG_ADDR_LOFF       0x04
#define REG_ADDR_CH1SET     0x05
#define REG_ADDR_CH2SET     0x06
#define REG_ADDR_CH3SET     0x07
#define REG_ADDR_CH4SET     0x08
#define REG_ADDR_CH5SET     0x09
#define REG_ADDR_CH6SET     0x0A
#define REG_ADDR_CH7SET     0x0B
#define REG_ADDR_CH8SET     0x0C
#define REG_ADDR_RLD_SENSP  0x0D
#define REG_ADDR_RLD_SENSN  0x0E
#define REG_ADDR_LOFF_SENSP 0x0F
#define REG_ADDR_LOFF_SENSN 0x10
#define REG_ADDR_LOFF_FLIP  0x11
#define REG_ADDR_LOFF_STATP 0x12
#define REG_ADDR_LOFF_STATN 0x13
#define REG_ADDR_GPIO       0x14
#define REG_ADDR_PACE       0x15
#define REG_ADDR_RESP       0x16
#define REG_ADDR_CONFIG4    0x17
#define REG_ADDR_WCT1       0x18
#define REG_ADDR_WCT2       0x19

// Times
#define T_CLK_NS 666    // Maximum clock period in nanoseconds, according to the datasheet
#define T_POR_MS 1000 // Time you should wait after power-on reset, acc. to the datasheet, in milliseconds



class ADS1298 {
public:
	SPI_HandleTypeDef *spi;

	ADS1298(SPI_HandleTypeDef *hspi);
	void setup();
	HAL_StatusTypeDef getChannelData(int32_t *channels);
	char* getChannelDataRaw();

	void getAllRegisters();
	void writeRegister(uint8_t registerAddr, uint8_t newValue);
	void writeRegisters(uint8_t startAddr, uint8_t* newValues);
	uint8_t readRegister(uint8_t registerAddr);
	uint8_t* readRegisters(uint8_t startAddr, uint8_t count);
	void sendCommand(uint8_t);

	void useTestSignal(uint8_t channelNo);
	void useRegularSignal(uint8_t channelNo);

	void useInternalReference();

	// Commands, as listed in the datasheet
	void RESET();
	void RDATAC();
	void SDATAC();

	uint8_t buf[27];
	uint8_t txbuf[27] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int32_t channelData[8];

	virtual ~ADS1298();
};

#endif /* SRC_ADS1298_HPP_ */
