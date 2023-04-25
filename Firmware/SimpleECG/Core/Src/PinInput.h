/*
 * PinInput.h
 *
 *  Created on: Apr 2, 2023
 *      Author: jojo
 */

#ifndef SRC_PININPUT_H_
#define SRC_PININPUT_H_

#include "stm32f1xx_hal.h"

class PinInput {
public:
	GPIO_TypeDef* _port;
	uint16_t _pin;

	bool high();
	bool low();

	PinInput(GPIO_TypeDef* port, uint16_t pin);;
	virtual ~PinInput();
};

#endif /* SRC_PININPUT_H_ */
