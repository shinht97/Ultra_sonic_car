/*
 * delay.c
 *
 *  Created on: Nov 1, 2023
 *      Author: Administrator1
 */

#include "delay.h"

void delay_us(uint16_t _us)
{
   __HAL_TIM_SET_COUNTER(&htim11, 0); // 타이머의 값을 초기화

   while((__HAL_TIM_GET_COUNTER(&htim11)) < _us) // 타이머의 카운터 값과 _us를 비교
   {
      // 의도적으로 while에 가둬 둠
   }
}
