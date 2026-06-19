/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "My_Code1.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for KEY_Task */
osThreadId_t KEY_TaskHandle;
const osThreadAttr_t KEY_Task_attributes = {
  .name = "KEY_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for LED_Task */
osThreadId_t LED_TaskHandle;
const osThreadAttr_t LED_Task_attributes = {
  .name = "LED_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for USART_Task */
osThreadId_t USART_TaskHandle;
const osThreadAttr_t USART_Task_attributes = {
  .name = "USART_Task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh1,
};
/* Definitions for OLED_Task */
osThreadId_t OLED_TaskHandle;
const osThreadAttr_t OLED_Task_attributes = {
  .name = "OLED_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for W25Q64_Task */
osThreadId_t W25Q64_TaskHandle;
const osThreadAttr_t W25Q64_Task_attributes = {
  .name = "W25Q64_Task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal2,
};
/* Definitions for PA11Flick_Task */
osThreadId_t PA11Flick_TaskHandle;
const osThreadAttr_t PA11Flick_Task_attributes = {
  .name = "PA11Flick_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for KEY_Queue */
osMessageQueueId_t KEY_QueueHandle;
const osMessageQueueAttr_t KEY_Queue_attributes = {
  .name = "KEY_Queue"
};
/* Definitions for USART_Queue */
osMessageQueueId_t USART_QueueHandle;
const osMessageQueueAttr_t USART_Queue_attributes = {
  .name = "USART_Queue"
};
/* Definitions for General_Mutex */
osMutexId_t General_MutexHandle;
const osMutexAttr_t General_Mutex_attributes = {
  .name = "General_Mutex"
};
/* Definitions for KEY_BinarySem */
osSemaphoreId_t KEY_BinarySemHandle;
const osSemaphoreAttr_t KEY_BinarySem_attributes = {
  .name = "KEY_BinarySem"
};
/* Definitions for USART_CountingSem */
osSemaphoreId_t USART_CountingSemHandle;
const osSemaphoreAttr_t USART_CountingSem_attributes = {
  .name = "USART_CountingSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void vIP_KEYTask(void *argument);
void vIP_LEDTask(void *argument);
void vIP_USARTTask(void *argument);
void vIP_OLEDTask(void *argument);
void vIP_W25Q64Task(void *argument);
void vIP_PA11Flick(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of General_Mutex */
  General_MutexHandle = osMutexNew(&General_Mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of KEY_BinarySem */
  KEY_BinarySemHandle = osSemaphoreNew(1, 0, &KEY_BinarySem_attributes);

  /* creation of USART_CountingSem */
  USART_CountingSemHandle = osSemaphoreNew(10, 0, &USART_CountingSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of KEY_Queue */
  KEY_QueueHandle = osMessageQueueNew (16, sizeof(KEYMessage), &KEY_Queue_attributes);

  /* creation of USART_Queue */
  USART_QueueHandle = osMessageQueueNew (16, sizeof(USARTMessage), &USART_Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of KEY_Task */
  KEY_TaskHandle = osThreadNew(vIP_KEYTask, NULL, &KEY_Task_attributes);

  /* creation of LED_Task */
  LED_TaskHandle = osThreadNew(vIP_LEDTask, NULL, &LED_Task_attributes);

  /* creation of USART_Task */
  USART_TaskHandle = osThreadNew(vIP_USARTTask, NULL, &USART_Task_attributes);

  /* creation of OLED_Task */
  OLED_TaskHandle = osThreadNew(vIP_OLEDTask, NULL, &OLED_Task_attributes);

  /* creation of W25Q64_Task */
  W25Q64_TaskHandle = osThreadNew(vIP_W25Q64Task, NULL, &W25Q64_Task_attributes);

  /* creation of PA11Flick_Task */
  PA11Flick_TaskHandle = osThreadNew(vIP_PA11Flick, NULL, &PA11Flick_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_vIP_KEYTask */
/**
  * @brief  Function implementing the KEY_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_vIP_KEYTask */
__weak void vIP_KEYTask(void *argument)
{
  /* USER CODE BEGIN vIP_KEYTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END vIP_KEYTask */
}

/* USER CODE BEGIN Header_vIP_LEDTask */
/**
* @brief Function implementing the LED_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vIP_LEDTask */
__weak void vIP_LEDTask(void *argument)
{
  /* USER CODE BEGIN vIP_LEDTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END vIP_LEDTask */
}

/* USER CODE BEGIN Header_vIP_USARTTask */
/**
* @brief Function implementing the USART_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vIP_USARTTask */
__weak void vIP_USARTTask(void *argument)
{
  /* USER CODE BEGIN vIP_USARTTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END vIP_USARTTask */
}

/* USER CODE BEGIN Header_vIP_OLEDTask */
/**
* @brief Function implementing the OLED_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vIP_OLEDTask */
__weak void vIP_OLEDTask(void *argument)
{
  /* USER CODE BEGIN vIP_OLEDTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END vIP_OLEDTask */
}

/* USER CODE BEGIN Header_vIP_W25Q64Task */
/**
* @brief Function implementing the W25Q64_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vIP_W25Q64Task */
__weak void vIP_W25Q64Task(void *argument)
{
  /* USER CODE BEGIN vIP_W25Q64Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END vIP_W25Q64Task */
}

/* USER CODE BEGIN Header_vIP_PA11Flick */
/**
* @brief Function implementing the PA11Flick_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vIP_PA11Flick */
__weak void vIP_PA11Flick(void *argument)
{
  /* USER CODE BEGIN vIP_PA11Flick */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END vIP_PA11Flick */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

