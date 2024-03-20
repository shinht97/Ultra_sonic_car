/*
 * Button.c
 *
 *  Created on: Feb 19, 2024
 *      Author: Administrator1
 */

#include "button.h"

void Button_Init(Button *_btn, GPIO_TypeDef *_GPIOx, uint16_t _GPIO_Pin)
{
	_btn->GPIOx = _GPIOx;
	_btn->GPIO_Pin = _GPIO_Pin;
	_btn->preState = RELEASED;
}

uint8_t Button_getState(Button *_btn)
{
	uint8_t curState = HAL_GPIO_ReadPin(_btn->GPIOx, _btn->GPIO_Pin);

	if((curState == PUSHED) && (_btn->preState == RELEASED))
	{
		HAL_Delay(10);
		_btn->preState = PUSHED;
		return ACT_PUSHED;
	}
	else if((curState != PUSHED) && (_btn->preState == PUSHED))
	{
		HAL_Delay(10);
		_btn->preState = RELEASED;
		return ACT_RELEASED;
	}

	return NO_ACT;
}
