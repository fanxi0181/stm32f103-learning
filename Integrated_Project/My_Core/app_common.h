// app_common.h 公共结构体 头文件
#ifndef INC_APP_COMMON_H_
#define INC_APP_COMMON_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "app_config.h"

#ifdef APP_COMMON_ENABLED

extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;
extern osSemaphoreId_t KEY_BinarySemHandle;//按键二进制信号量 通知按键已经按下
extern osSemaphoreId_t USART_CountingSemHandle;//串口计数信号量
extern osMutexId_t General_MutexHandle;//通用互斥锁
extern osMessageQueueId_t KEY_QueueHandle;//按键队列 传递W25Q64状态
extern osMessageQueueId_t USART_QueueHandle;//串口队列 传递LED状态 OLED状态

//——————————枚举区 开始——————————
//led状态枚举
typedef enum{
    LED_State_ON,
    LED_State_OFF,
    LED_State_Flicker
}LED_State;

//w25q64状态枚举
typedef enum{
    W25Q64_State_Save,
    W25Q64_State_Recovery
}W25Q64_State;

//oled状态枚举
typedef enum{
    OLED_State_Display1,//显示 LED:ON
    OLED_State_Display2,//显示 LED:OFF
    OLED_State_Display3//显示 STM32:0K
}OLED_State;
//——————————枚举区 结束——————————


//——————————队列区 开始——————————
//串口队列 发送led状态，oled状态，等等……
typedef struct{
    LED_State led_state;
    OLED_State oled_state;
}USARTMessage;

//按键队列，发送w25q64状态，等等……
typedef struct{
    W25Q64_State w25q64_state;
}KEYMessage;
//——————————队列区 结束——————————


//APP状态 所有状态皆可放到此处
typedef struct{
    LED_State led;//led状态
    OLED_State oled;//oled状态
    W25Q64_State w25q64;//w25q64状态
}AppState;

extern KEYMessage keymessage;//按键队列
extern USARTMessage usartmessage;//串口队列
extern volatile AppState g_state;//全局APP状态

#endif //APP_COMMON_ENABLED

#endif
