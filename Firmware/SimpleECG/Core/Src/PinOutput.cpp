/*
 * PinOutput.cpp
 *
 *  Created on: Apr 2, 2023
 *      Author: jojo
 */

#include "PinOutput.hpp"

PinOutput::PinOutput(GPIO_TypeDef* port, uint16_t pin) {
	_port = port;
	_pin = pin;

	// Define GPIO pin configuration parameters
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	// Initialize GPIO pin
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void PinOutput::high() {
	HAL_GPIO_WritePin(PinOutput::_port, PinOutput::_pin, GPIO_PIN_SET);
}

void PinOutput::low() {
	HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);
}

PinOutput::~PinOutput() {
	// TODO Auto-generated destructor stub
}

