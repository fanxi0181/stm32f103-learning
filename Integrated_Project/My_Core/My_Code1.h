/*
 * My_Code1.h
 *
 *  Created on: Jun 18, 2026
 *      Author: lin
 */

#ifndef INC_MY_CODE1_H_
#define INC_MY_CODE1_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "OLED.h"

// #include "FreeRTOS.h"
// #include "task.h"
// #include "queue.h"

extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern osSemaphoreId_t KEY_BinarySemHandle;//按键二进制信号量 通知按键已经按下
extern osSemaphoreId_t USART_CountingSemHandle;//串口计数信号量
extern osMutexId_t General_MutexHandle;//通用互斥锁
extern osMessageQueueId_t KEY_QueueHandle;//按键队列 传递W25Q64状态
extern osMessageQueueId_t USART_QueueHandle;//串口队列 传递LED状态 OLED状态

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

//串口队列 发送led状态，oled状态，等等……
typedef struct{
    LED_State led_state;
    OLED_State oled_state;
}USARTMessage;

//按键队列，发送w25q64状态，等等……
typedef struct{
    W25Q64_State w25q64_state;
}KEYMessage;

//APP状态 所有状态皆可放到此处
typedef struct{
    LED_State led;//led状态
    OLED_State oled;//oled状态
    W25Q64_State w25q64;//w25q64状态
}AppState;

extern volatile AppState g_state;//全局APP状态
extern KEYMessage keymessage;//按键队列
extern USARTMessage usartmessage;//串口队列

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);//按键下降沿触发中断函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);//串口接收到数据回调

void vIP_KEYTask(void *argument);//按键任务
void vIP_LEDTask(void *argument);//led任务，同时处理PA3的led状态 和 oled状态刷新
void vIP_USARTTask(void *argument);//串口任务，负责处理接收到的字符 并转移到led任务
void vIP_OLEDTask(void *argument);//oled任务，只初始化显示开屏动画
void vIP_W25Q64Task(void *argument);//w25q64任务 负责处理保存/恢复
void vIP_PA11Flick(void *argument);//常态PA11引脚的蓝色小灯闪烁

void Init_OLED(void);//测试oled

void UART1_Receive_Start(void);//启动串口1中断接收
void W25Q64_Save_State(uint8_t State);//w25q64保存
uint8_t W25Q64_Read_ME(void);//w25q64读取
void W25Q64_WaitBusy(void);//w25q64等待空闲


#endif /* INC_MY_CODE1_H_ */
