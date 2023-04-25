/*
 * PinInput.cpp
 *
 *  Created on: Apr 2, 2023
 *      Author: jojo
 */

#include "PinInput.h"

PinInput::PinInput(GPIO_TypeDef* port, uint16_t pin) {
	_port = port;
	_pin = pin;

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = pin; // Replace X with the GPIO pin number you want to use
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP; // Replace with GPIO_PULLDOWN if you want a pull-down resistor
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // Replace with the appropriate speed for your application

	HAL_GPIO_Init(port, &GPIO_InitStruct);
}

bool PinInput::high() {
	return HAL_GPIO_ReadPin(PinInput::_port, PinInput::_pin) == GPIO_PIN_SET;
}

bool PinInput::low() {
	return HAL_GPIO_ReadPin(PinInput::_port, PinInput::_pin) == GPIO_PIN_RESET;
}

PinInput::~PinInput() {
	// TODO Auto-generated destructor stub
}

