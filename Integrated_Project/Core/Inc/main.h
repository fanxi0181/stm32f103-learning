/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PC13_LED3_Pin GPIO_PIN_13
#define PC13_LED3_GPIO_Port GPIOC
#define PA1_KEY1_Pin GPIO_PIN_1
#define PA1_KEY1_GPIO_Port GPIOA
#define PA1_KEY1_EXTI_IRQn EXTI1_IRQn
#define PA2_KEY2_Pin GPIO_PIN_2
#define PA2_KEY2_GPIO_Port GPIOA
#define PA2_KEY2_EXTI_IRQn EXTI2_IRQn
#define PA3_LED1_Pin GPIO_PIN_3
#define PA3_LED1_GPIO_Port GPIOA
#define PA4_W25Q64_Pin GPIO_PIN_4
#define PA4_W25Q64_GPIO_Port GPIOA
#define PB0_KEY3_Pin GPIO_PIN_0
#define PB0_KEY3_GPIO_Port GPIOB
#define PA11_LED2_Pin GPIO_PIN_11
#define PA11_LED2_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
