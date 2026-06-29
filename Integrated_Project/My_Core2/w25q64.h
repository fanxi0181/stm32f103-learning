// w25q64.h —— W25Q64 SPI Flash 驱动（保存/恢复 LED 状态）
#ifndef INC_W25Q64_H_
#define INC_W25Q64_H_

#include "app_common.h"
#include "spi.h"

#ifdef W25Q64_MODULE_ENABLED

void    vIP_W25Q64Task(void *argument);  // w25q64任务
void    W25Q64_Save_All(void *data);     // 写入结构体到扇区0
void    W25Q64_Read_All(void *data);     // 从扇区0读取结构体
bool    W25Q64_Erase_Config_Sector(void);// 擦除扇区0
void    W25Q64_WaitBusy(void);           // 等待 Flash 空闲

#else
#define W25Q64_Save_All(data)           
#define W25Q64_Read_All(data)
#define W25Q64_Erase_Config_Sector() false
#define W25Q64_WaitBusy()

#endif //W25Q64_MODULE_ENABLED

#endif //INC_W25Q64_H_
