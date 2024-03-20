/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "stdio.h"
#include "math.h"
#include "usart.h"
#include "Button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xFFFF);
  HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 0xFFFF);

  return ch;
}

//#define babybird
#define eagle
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint32_t INC_Value11 = 0;
uint32_t INC_Value12 = 0;

uint32_t INC_Value21 = 0;
uint32_t INC_Value22= 0;

uint32_t INC_Value31= 0;
uint32_t INC_Value32 = 0;

uint32_t echoTime1 = 0;
uint32_t echoTime2 = 0;
uint32_t echoTime3 = 0;

uint8_t captureFlag1 = 0;
uint8_t captureFlag2 = 0;
uint8_t captureFlag3 = 0;

uint8_t distance1  = 0;
uint8_t distance2  = 0;
uint8_t distance3  = 0;

uint8_t rxData[1];

#ifdef babybird
int mode = 1;
int SPEED = 300;
#endif

#ifdef eagle
int mode = 0;
int SPEED = 480;
#endif

volatile uint32_t counter;

int set_back_flag = 0;
char flag[15] = "INIT";

int cnt = 0;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for distance1 */
osThreadId_t distance1Handle;
const osThreadAttr_t distance1_attributes = {
  .name = "distance1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for distance2 */
osThreadId_t distance2Handle;
const osThreadAttr_t distance2_attributes = {
  .name = "distance2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for distance3 */
osThreadId_t distance3Handle;
const osThreadAttr_t distance3_attributes = {
  .name = "distance3",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3)  // if the interrupt source is channel1
    {
        if (captureFlag1 == 0) // if the first value is not captured
        {
          INC_Value11 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
          captureFlag1 = 1;  // set the first captured as true

            // Now change the polarity to falling edge
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        }

        else if (captureFlag1 == 1)   // if the first is already captured
        {
          INC_Value12 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
            __HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

            if (INC_Value12 > INC_Value11)
            {
              echoTime1 = INC_Value12-INC_Value11;
            }

            else if (INC_Value11 > INC_Value12)
            {
              echoTime1 = (0xffff - INC_Value11) + INC_Value12;
            }

            //distance = echoTime * .034/2;
            distance1 = echoTime1 / 58;
            captureFlag1 = 0; // set it back to false

            // set polarity to rising edge
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
        }
    }
    else if (htim == &htim4)  // if the interrupt source is channel1
	{
		if (captureFlag2 == 0) // if the first value is not captured
		{
		  INC_Value21 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
		  captureFlag2 = 1;  // set the first captured as true

			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (captureFlag2 == 1)   // if the first is already captured
		{
		  INC_Value22 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (INC_Value22 > INC_Value21)
			{
			  echoTime2 = INC_Value22 - INC_Value21;
			}

			else if (INC_Value21 > INC_Value22)
			{
			  echoTime2 = (0xffff - INC_Value21) + INC_Value22;
			}

			//distance = echoTime * .034/2;
			distance2 = echoTime2 / 58;
			captureFlag2 = 0; // set it back to false

			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
		}
	}
    else if (htim == &htim5)  // if the interrupt source is channel1
	{
		if (captureFlag3 == 0) // if the first value is not captured
		{
		    INC_Value31 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
		    captureFlag3 = 1;  // set the first captured as true

			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (captureFlag3 == 1)   // if the first is already captured
		{
		    INC_Value32 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (INC_Value32 > INC_Value31)
			{
			  echoTime3 = INC_Value32-INC_Value31;
			}

			else if (INC_Value31 > INC_Value32)
			{
			  echoTime3 = (0xffff - INC_Value31) + INC_Value32;
			}

			//distance = echoTime * .034/2;
			distance3 = echoTime3 / 58;
			captureFlag3 = 0; // set it back to false

			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
		}
	}
}

void HCSR04_Read (GPIO_TypeDef* port, uint16_t pin, TIM_HandleTypeDef* htim)
{
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);  // pull the TRIG pin HIGH
    delay_us(10);  // wait for 10 us
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);  // pull the TRIG pin low

    __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		HAL_UART_Receive_DMA(&huart1, rxData, sizeof(rxData));

		if(rxData[0] == 'M')
		{
		   mode = 1;
		}
		else if(rxData[0] == 'A')
		{
			mode = 0;
		}
		else if(rxData[0] == 'u')
		{
			SPEED += 10;

			if (SPEED > 1000)
			{
				SPEED = 999;
			 }
		}
		else if(rxData[0] == 'i')
		{
			SPEED -= 10;

			if (SPEED < 0)
			{
			  SPEED = 0;
			}
		}
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void distanceTask1(void *argument);
void distanceTask2(void *argument);
void distanceTask3(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of distance1 */
  distance1Handle = osThreadNew(distanceTask1, NULL, &distance1_attributes);

  /* creation of distance2 */
  distance2Handle = osThreadNew(distanceTask2, NULL, &distance2_attributes);

  /* creation of distance3 */
  distance3Handle = osThreadNew(distanceTask3, NULL, &distance3_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
#ifdef eagle
	Button btn;

	Button_Init(&btn, GPIOC, GPIO_PIN_3);
#endif

  int set_count_flag = 0;
  /* Infinite loop */
  for(;;)
  {

#ifdef eagle
	  if (Button_getState(&btn) == ACT_PUSHED)
	  {
		  mode = (mode + 1) % 2;
	  }
#endif

	  if (((__HAL_TIM_GET_COUNTER(&htim10) % 1000) * 1000) % 1000 == 0)
	  {
		  printf("speed left : %d speed right : %d\r\n", TIM1->CCR1, TIM2->CCR1);
	  }

	  if (mode == 0)
	  {
		  if(rxData[0] == 'w')
		  {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

			  TIM1->CCR1 = SPEED;

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);

			  TIM2->CCR1 = SPEED;
		  }
		  else if(rxData[0] == 's')
		  {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);

			  TIM1->CCR1 = SPEED;

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);

			  TIM2->CCR1 = SPEED;
			}
			else if(rxData[0] == 'a')
			{
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

			  TIM1->CCR1 = SPEED;

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);

			  TIM2->CCR1 = SPEED;
			}
			else if(rxData[0] == 'd')
			{
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

			  TIM1->CCR1 = SPEED;

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);

			  TIM2->CCR1 = SPEED;
			}
			else if(rxData[0] == 'k')
			{
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
			}
			else if(rxData[0] == 0)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
			}
	  }
	  else
	  {
#ifdef eagle
//		  if (distance1 > 50)
//		  {
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//			  TIM1->CCR1 = SPEED;
//
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//			  TIM2->CCR1 = SPEED;
//
//			  if (distance2 < 18)
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//				  TIM1->CCR1 = SPEED;
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//				  TIM2->CCR1 = SPEED;
//			  }
//
//			  if (distance3 < 18)
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//				  TIM1->CCR1 = SPEED;
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//				  TIM2->CCR1 = SPEED;
//			  }
//		  }
//		  else if (distance1 <= 50 && distance1 > 20)
//		  {
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//			  TIM1->CCR1 = (SPEED * distance1) * 0.02;
//
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//			  TIM2->CCR1 = (SPEED * distance1 ) * 0.02;
//
//			  if (distance2 < 18)
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//				  TIM1->CCR1 = (SPEED * distance1) * 0.02;
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
//
//				  TIM2->CCR1 = (SPEED * distance1) * 0.02;
//			  }
//
//			  if (distance3 < 18)
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
//
//				  TIM1->CCR1 = (SPEED * distance1) * 0.02;
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//				  TIM2->CCR1 = (SPEED * distance1) * 0.02;
//			  }
//		  }
//		  else if (distance1 <= 20)
//		  {
//			  if (distance2 > distance3)
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//				  TIM1->CCR1 = (SPEED + 300);
//
//				  TIM2->CCR1 = (SPEED - 150);
//			  }
//			  else if (distance2 < distance3)
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
//
//				  TIM1->CCR1 = (SPEED - 150);
//
//				  TIM2->CCR1 = (SPEED + 300);
//			  }
//		  }

		  if (distance1 < 25)
		  {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);

			  if (distance2 > distance3)
			  {
				  TIM1->CCR1 = SPEED * 1.1; // 550

				  TIM2->CCR1 = SPEED * 0.3; // 150
			  }
			  else if (distance2 < distance3)
			  {
				  TIM1->CCR1 = SPEED * 0.3;

				  TIM2->CCR1 = SPEED * 1.1;
			  }
		  }
		  else
		  {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);


			  if (distance2 < 17)
			  {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
			  }

			  if (distance3 < 17)
			  {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);

				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
			  }

			  TIM1->CCR1 = SPEED;

			  TIM2->CCR1 = SPEED;
		  }
#endif

#ifdef babybird

		  if (distance1 < 26)
		  {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);

			  if (distance2 > distance3)
			  {
				  TIM1->CCR1 = SPEED + 100;

				  TIM2->CCR1 = SPEED - 150; // 150
			  }
			  else if (distance2 < distance3)
			  {
				  TIM1->CCR1 = SPEED - 150;

				  TIM2->CCR1 = SPEED + 100;
			  }
		  }
		  else
		  {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);

			  if (distance2 < 18)
			  {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
			  }

			  if (distance3 < 18)
			  {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);

				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
			  }
			  TIM1->CCR1 = SPEED;
			  TIM2->CCR1 = SPEED;
		  }
#endif

//		  if (set_back_flag == 1)
//		  {
//			  // left
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
//
//			  TIM1->CCR1 = SPEED * 1.5;
//
//			  // right
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
//
//			  TIM2->CCR1 = SPEED * 1.5;
//
//			  if (distance2 > distance3)
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//				  TIM1->CCR1 = SPEED;
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//				  TIM2->CCR1 = SPEED * 1.5;
//			  }
//			  else if (distance2 < distance3)
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//				  TIM1->CCR1 = SPEED * 1.5;
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//				  TIM2->CCR1 = SPEED;
//			  }
//
//			  cnt = 0;
//			  set_back_flag = 0;
//		  }
//		  else
//		  {
//			  if (distance1 < 18)
//			  {
//				  // left
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
//
//				  TIM1->CCR1 = SPEED * 1.2;
//
//				  // right
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
//
//				  TIM2->CCR1 = SPEED * 1.2;
//
//				  set_count_flag = 1;
//			  }
//			  else
//			  {
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//				  TIM1->CCR1 = SPEED;
//
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//				  TIM2->CCR1 = SPEED;
//
//				  if (distance2 < 18)
//				  {
//					  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
//					  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//					  TIM1->CCR1 = SPEED;
//
//					  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
//					  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//					  TIM2->CCR1 = SPEED;
//				  }
//
//				  if (distance3 < 18)
//				  {
//					  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
//					  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
//
//					  TIM1->CCR1 = SPEED;
//
//					  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
//					  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
//
//					  TIM2->CCR1 = SPEED;
//				  }
//
//				  if (set_count_flag)
//				  {
//					  set_count_flag = 0;
//					  cnt++;
//				  }
//			  }
//
//			  if (cnt > 3)
//			  {
//				  set_back_flag = 1;
//			  }
//		  }
	  }

	  osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_distanceTask1 */
/**
* @brief Function implementing the distance1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_distanceTask1 */
void distanceTask1(void *argument)
{
  /* USER CODE BEGIN distanceTask1 */
  /* Infinite loop */
  for(;;)
  {
	  HCSR04_Read(GPIOA, GPIO_PIN_7, &htim3);

	  osDelay(50);
  }
  /* USER CODE END distanceTask1 */
}

/* USER CODE BEGIN Header_distanceTask2 */
/**
* @brief Function implementing the distance2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_distanceTask2 */
void distanceTask2(void *argument)
{
  /* USER CODE BEGIN distanceTask2 */
  /* Infinite loop */
  for(;;)
  {
	  HCSR04_Read(GPIOC, GPIO_PIN_7, &htim4);

	  osDelay(50);
  }
  /* USER CODE END distanceTask2 */
}

/* USER CODE BEGIN Header_distanceTask3 */
/**
* @brief Function implementing the distance3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_distanceTask3 */
void distanceTask3(void *argument)
{
  /* USER CODE BEGIN distanceTask3 */
  /* Infinite loop */
  for(;;)
  {
	  HCSR04_Read(GPIOA, GPIO_PIN_1, &htim5);

	  osDelay(50);
  }
  /* USER CODE END distanceTask3 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

