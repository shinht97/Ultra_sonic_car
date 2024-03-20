/*
 * Button.h
 *
 *  Created on: Feb 19, 2024
 *      Author: Administrator1
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "stm32f4xx_hal.h"

typedef struct _button
{
	GPIO_TypeDef *GPIOx; // same as avr ddr
	uint16_t	GPIO_Pin;
	uint16_t	preState;
}Button;

enum {PUSHED, RELEASED};
enum {NO_ACT, ACT_PUSHED, ACT_RELEASED};

void Button_Init(Button *_btn, GPIO_TypeDef *_GPIOx, uint16_t _GPIO_Pin);

uint8_t Button_getState(Button *_btn);

#endif /* INC_BUTTON_H_ */
