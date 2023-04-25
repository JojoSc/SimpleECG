/*
 * ADS1298.cpp
 *
 *  Created on: Apr 2, 2023
 *      Author: Johannes S.
 */

#include "ADS1298.hpp"


PinOutput nCS = PinOutput(nCS_GPIO_Port, nCS_Pin);
PinOutput nRESET = PinOutput(nRESET_GPIO_Port, nRESET_Pin);
PinOutput START = PinOutput(START_GPIO_Port, START_Pin);

uint8_t* registers;

void wait_ns(uint32_t ns) {
    // Enable the cycle counter
    DWT->CTRL |= (1 << DWT_CTRL_CYCCNTENA_Pos) | DWT_CTRL_CYCCNTENA_Msk;

    // Calculate the number of cycles needed to wait for the specified number of nanoseconds
    uint32_t cycles = SystemCoreClock / 1000000000 * ns;

    // Wait for the specified number of cycles
    uint32_t start = DWT->CYCCNT;
    while (DWT->CYCCNT - start < cycles);
}

void wait_us(uint32_t us) {
	wait_ns(us*1000);
}

ADS1298::ADS1298(SPI_HandleTypeDef *hspi) {
	spi = hspi;
}

ADS1298::~ADS1298() {
    // destructor code here
}


void ADS1298::RESET() {
    //nCS.low();

    nRESET.low();
    wait_ns(2 * T_CLK_NS);
    HAL_Delay(1);
    nRESET.high();
    //wait_ns(18 * T_CLK_NS);
    HAL_Delay(1);

    //nCS.high();
}

void ADS1298::SDATAC() {
    sendCommand(CMD_SDATAC);
    wait_ns(4 * T_CLK_NS);
}

void ADS1298::RDATAC() {
    sendCommand(CMD_RDATAC);
    wait_ns(4 * T_CLK_NS);
}

void ADS1298::sendCommand(uint8_t cmd) {
    //nCS.low();

    uint8_t tx[1] = { cmd };
    HAL_SPI_Transmit(spi, tx, 1, 100);
    while(HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);

    wait_ns(4 * T_CLK_NS);

    //nCS.high();
}

void ADS1298::useInternalReference() {
    ADS1298::writeRegister(REG_ADDR_CONFIG3, 0xC0);
}

void ADS1298::useTestSignal(uint8_t channelNo) {
    writeRegister(REG_ADDR_CONFIG2, 0x10);
    writeRegister(REG_ADDR_CH1SET + channelNo - 1, 0x05);
}

void ADS1298::useRegularSignal(uint8_t channelNo) {
    writeRegister(REG_ADDR_CONFIG2, 0x02);
    writeRegister(REG_ADDR_CH1SET + channelNo - 1, 0x00);
    writeRegister(REG_ADDR_CONFIG3, 0b1100'1100);
    writeRegister(REG_ADDR_RLD_SENSP, 0b0000'0011);
}

void ADS1298::setup()
{
    nRESET.high();
    nCS.low(); // TODO: Is something wrong with the toggling of CS? seems fucked
    HAL_Delay(T_POR_MS);
    RESET();
    SDATAC();

    useInternalReference();
    writeRegister(REG_ADDR_CONFIG1, 0x85); // high resolution, 1kSPS
    //useTestSignal(1);
    useRegularSignal(1);
    useRegularSignal(2);
    useRegularSignal(3);
    useRegularSignal(4);


    START.high();
    RDATAC();
}

void ADS1298::writeRegister(uint8_t registerAddr, uint8_t newValue) {
    //nCS.low();

    uint8_t tx = CMD_WREG | registerAddr;
    HAL_SPI_Transmit(spi, &tx, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);

    wait_ns(4 * T_CLK_NS);

    tx = 0x00;
    HAL_SPI_Transmit(spi, &tx, 1, HAL_MAX_DELAY); // 0x00 means only one register, not multiple
    while(HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);

    wait_ns(4 * T_CLK_NS);

    tx = newValue;
    HAL_SPI_Transmit(spi, &tx, 1, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);

    wait_ns(4 * T_CLK_NS);

    //nCS.high();
}

uint8_t ADS1298::readRegister(uint8_t registerAddr)
{
	//nCS.low();

    HAL_StatusTypeDef hal_status;
    uint8_t tx_data[3] = {(uint8_t)(CMD_RREG | registerAddr), 0x00, 0x00};
    uint8_t rx_data[3];


    hal_status = HAL_SPI_TransmitReceive(spi, tx_data, rx_data, 3, HAL_MAX_DELAY);
    while(HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);

    //nCS.high();
    if (hal_status == HAL_OK)
    {
        return rx_data[2];    // response is in the second byte
    }
    else {
    	return 0xFF;
    }
}

int32_t getBit(int32_t x, uint32_t i) {
    return (x & (1 << i)) >> i;
}

HAL_StatusTypeDef ADS1298::getChannelData(int32_t *channels) {
    //nCS.low();
	HAL_StatusTypeDef hal_status;
	uint8_t tx_data[27];
	uint8_t rx_data[27];
	for (int i = 0; i < 27; i++) {
		tx_data[i] = 0x00;
		rx_data[i] = 0x00;
	}


	hal_status = HAL_SPI_TransmitReceive(spi, tx_data, rx_data, 27, HAL_MAX_DELAY);
	while(HAL_SPI_GetState(spi) != HAL_SPI_STATE_READY);

    for (int i = 0; i < 8; i++) {
    	channels[i] = 0 | (rx_data[3 + 3*i] << 16) | (rx_data[4 + 3*i] << 8) | rx_data[5 + 3*i];

        if (getBit(channels[i], 23) == 1) {
        	channels[i] = channels[i] | 0xFF000000;
        }
    }

    //nCS.high();
    return hal_status;
}

//uint8_t* ADS1298::readRegisters(uint8_t startAddr, uint8_t count) {
//    //nCS.low();
//
//    spi.write(CMD_RREG | startAddr);
//    wait_ns(4 * T_CLK_NS);
//    spi.write(count - 1);
//    wait_ns(4 * T_CLK_NS);
//
//    uint8_t regVals[count];
//    for (int i = 0; i < count; i++) {
//        regVals[i] = spi.write(CMD_NOP);
//        wait_ns(4 * T_CLK_NS);
//    }
//
//    //nCS.high();
//    return regVals;
//}
//
//void ADS1298::getAllRegisters() {
//    registers = readRegisters(0x00, 0x18);
//}
//

//
//
//void SPI_Receive (uint8_t *data, int size) {
//    // https://controllerstech.com/spi-using-registers-in-stm32/
//	/************** STEPS TO FOLLOW *****************
//	1. Wait for the BSY bit to reset in Status Register
//	2. Send some Dummy data before reading the DATA
//	3. Wait for the RXNE bit to Set in the status Register
//	4. Read data from Data Register
//	************************************************/
//
//	while (size)
//	{
//		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
//		SPI1->DR = 0;  // send dummy data
//		while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
//	    *data++ = (SPI1->DR);
//		size--;
//	}
//}
//

//
//char* ADS1298::getChannelDataRaw() {
//    //nCS.low();
//    SPI_Receive(buf, 27);
//    //nCS.high();
//    return (char*)buf;
//}
