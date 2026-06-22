//My_Code1.h

#ifndef INC_MY_CODE1_H_
#define INC_MY_CODE1_H_

#include "app_common.h"
#include "OLED.h"

#ifdef SIX_MODULE_LINGKAGE_FREERTOS_ENABLED

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

#endif //SIX_MODULE_LINGKAGE_FREERTOS

#endif /* INC_MY_CODE1_H_ */
