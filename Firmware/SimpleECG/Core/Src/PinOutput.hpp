/*
 * PinOutput.h
 *
 *  Created on: Apr 2, 2023
 *      Author: jojo
 */

#ifndef SRC_PINOUTPUT_HPP_
#define SRC_PINOUTPUT_HPP_

#include "stm32f1xx_hal.h"

class PinOutput {
public:
	GPIO_TypeDef* _port;
	uint16_t _pin;

	PinOutput(GPIO_TypeDef* port, uint16_t pin);
	void high();
	void low();
	virtual ~PinOutput();
};

#endif /* SRC_PINOUTPUT_HPP_ */
