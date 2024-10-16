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
#include <stdint.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
typedef struct
{
	uint16_t Value;
	uint8_t Source;
} Data;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
Data DataToSend1={'a', 1};
Data DataToSend2={'b', 2};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for helloTask */
osThreadId_t helloTaskHandle;
const osThreadAttr_t helloTask_attributes = {
  .name = "helloTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for senderTask */
osThreadId_t senderTaskHandle;
const osThreadAttr_t senderTask_attributes = {
  .name = "senderTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for QueueSimpleData */
osMessageQueueId_t QueueSimpleDataHandle;
const osMessageQueueAttr_t QueueSimpleData_attributes = {
  .name = "QueueSimpleData"
};
/* Definitions for QueueStructData */
osMessageQueueId_t QueueStructDataHandle;
const osMessageQueueAttr_t QueueStructData_attributes = {
  .name = "QueueStructData"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartHelloTask(void *argument);
void StartSenderTask(void *argument);

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

  /* Create the queue(s) */
  /* creation of QueueSimpleData */
  QueueSimpleDataHandle = osMessageQueueNew (8, sizeof(uint8_t), &QueueSimpleData_attributes);

  /* creation of QueueStructData */
  QueueStructDataHandle = osMessageQueueNew (8, sizeof(Data), &QueueStructData_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of helloTask */
  helloTaskHandle = osThreadNew(StartHelloTask, NULL, &helloTask_attributes);

  /* creation of senderTask */
  senderTaskHandle = osThreadNew(StartSenderTask, NULL, &senderTask_attributes);

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
  uint8_t x=0;
  uint32_t queue_cap = osMessageQueueGetCapacity(QueueSimpleDataHandle);
  uint32_t num_in_queue, space_in_queue;
  osStatus_t s_state;
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	num_in_queue = osMessageQueueGetCount(QueueSimpleDataHandle);
	space_in_queue = osMessageQueueGetSpace(QueueSimpleDataHandle);
	s_state = osMessageQueuePut(QueueSimpleDataHandle, &x, 0, 100);
	if(s_state == osOK)
	{
		if(++x>9)
		{
			x=0;
		}
	}
	else
	{
		printf("osMessageQueuePut Timeout\n");
	}
	printf("Queue Stats (%u:%u) Space %u\n", num_in_queue, queue_cap, space_in_queue);

	s_state = osMessageQueuePut(QueueStructDataHandle, &DataToSend1, 0, 100);
	if(s_state != osOK)
	{
		printf("osMessageQueuePut Timeout\n");
	}

    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartHelloTask */
/**
* @brief Function implementing the helloTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartHelloTask */
void StartHelloTask(void *argument)
{
  /* USER CODE BEGIN StartHelloTask */
  uint8_t rec_x;
  Data retvalue;
  osStatus_t r_state;
  /* Infinite loop */
  for(;;)
  {
	r_state = osMessageQueueGet(QueueSimpleDataHandle, &rec_x, 0, 0);
	if(r_state == osOK)
	{
		printf("Hello RTOS %d\n",rec_x);
	}
	else
	{
//		printf("osMessageQueueGet Timeout\n");
	}

	r_state = osMessageQueueGet(QueueStructDataHandle, &retvalue, 0, 0);
	if(r_state == osOK)
	{
		printf("Struct Data: %c, %u\n",retvalue.Value, retvalue.Source);
	}
	else
	{
//		printf("osMessageQueueGet Timeout\n");
	}

    osDelay(100);
  }
  /* USER CODE END StartHelloTask */
}

/* USER CODE BEGIN Header_StartSenderTask */
/**
* @brief Function implementing the senderTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSenderTask */
void StartSenderTask(void *argument)
{
  /* USER CODE BEGIN StartSenderTask */
  /* Infinite loop */
  for(;;)
  {
	osMessageQueuePut(QueueStructDataHandle, &DataToSend2, 0, 200);
    osDelay(2000);
  }
  /* USER CODE END StartSenderTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

