// w25q64.c —— W25Q64 SPI Flash 驱动：
//   vIP_W25Q64Task       ：上电/Load_Config 恢复，按键队列触发 Save/Load
//   W25Q64_Save_All/Read_All/WaitBusy ：底层 SPI 读写擦除

#include "w25q64.h"
#include "config_service.h"
#include "uart_dma.h"

#ifdef W25Q64_MODULE_ENABLED

#define W25Q64_SAVE_KEY_PORT PA3_LED1_GPIO_Port
#define W25Q64_SAVE_KEY_PIN PA3_LED1_Pin

#define W25Q64_CS_PORT PA4_W25Q64_GPIO_Port
#define W25Q64_CS_PIN PA4_W25Q64_Pin
#define W25Q64_HANDLE hspi1

// 等待 KEY_Queue 消息 → Save_Config 或 Load_Config
// 使用config_service 中的函数进行保存与恢复 此文件只进行flash操作
void vIP_W25Q64Task(void *argument)
{
    /* ---- 上电恢复（只执行一次） ---- */
    Config_Load();

    for(;;)
    {
        KEYMessage msg;
        if(osMessageQueueGet(KEY_QueueHandle, &msg, 0, osWaitForever) == osOK)
        {
            if(msg.w25q64_state == W25Q64_State_Save)
            {
                Config_Save();   // 保存当前全部配置
            }
            else if(msg.w25q64_state == W25Q64_State_Recovery)
            {
                Config_Load();   // 从 Flash 恢复配置
            }
        }
        osDelay(pdMS_TO_TICKS(10));
    }
}

// 擦除配置扇区（扇区 0），用于 "w25q64 clear" 命令
bool W25Q64_Erase_Config_Sector(void)
{
    uint8_t cmd;

    // 1. 写使能
    cmd = 0x06;
    HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&W25Q64_HANDLE, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN, GPIO_PIN_SET);

    // 2. 扇区擦除命令（0x20），地址 0x000000
    uint8_t erase_cmd[4] = {0x20, 0x00, 0x00, 0x00};
    HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&W25Q64_HANDLE, erase_cmd, 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN, GPIO_PIN_SET);

    // 3. 等待擦除完成
    W25Q64_WaitBusy();

    return true;  // 可后续增加状态寄存器检查以确认成功
}

// 保存 结构体到 Flash：写使能 → 擦除扇区0 → 写使能 → 写入
void W25Q64_Save_All(void *data)
{
    uint8_t Buffer[5];
    Buffer[0] = 0x06;//写使能
    HAL_GPIO_WritePin(W25Q64_CS_PORT,W25Q64_CS_PIN ,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&W25Q64_HANDLE,Buffer,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(W25Q64_CS_PORT,W25Q64_CS_PIN ,GPIO_PIN_SET);

    Buffer[0] = 0x20;//扇擦除命令
	Buffer[1] = 0x00;
	Buffer[2] = 0x00;
	Buffer[3] = 0x00;
    HAL_GPIO_WritePin(W25Q64_CS_PORT,W25Q64_CS_PIN ,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&W25Q64_HANDLE,Buffer,4,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(W25Q64_CS_PORT,W25Q64_CS_PIN ,GPIO_PIN_SET);
    
    W25Q64_WaitBusy(); 

    Buffer[0] = 0x06;
    HAL_GPIO_WritePin(W25Q64_CS_PORT,W25Q64_CS_PIN ,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&W25Q64_HANDLE,Buffer,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(W25Q64_CS_PORT,W25Q64_CS_PIN ,GPIO_PIN_SET);

	Buffer[0] = 0x02;//页编程
	Buffer[1] = 0x00;
	Buffer[2] = 0x00;
	Buffer[3] = 0x00;
    HAL_GPIO_WritePin(W25Q64_CS_PORT,W25Q64_CS_PIN ,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&W25Q64_HANDLE,Buffer,4,HAL_MAX_DELAY);
    HAL_SPI_Transmit(&W25Q64_HANDLE,(uint8_t*)data,sizeof(W25Q64_Config),HAL_MAX_DELAY);
    HAL_GPIO_WritePin(W25Q64_CS_PORT,W25Q64_CS_PIN ,GPIO_PIN_SET);
    
    W25Q64_WaitBusy(); 

    osDelay(pdMS_TO_TICKS(5));
}

// 读取扇区结构体
void W25Q64_Read_All(void *data)
{
    uint8_t cmd[4] = {0x03,0x00,0x00,0x00};
    W25Q64_WaitBusy(); 
    HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN , GPIO_PIN_RESET);
    HAL_SPI_Transmit(&W25Q64_HANDLE, cmd, 4, HAL_MAX_DELAY);   // 发送读命令
    HAL_SPI_Receive(&W25Q64_HANDLE,(uint8_t*)data, sizeof(W25Q64_Config), HAL_MAX_DELAY);    // HAL会自动发OxFF接收数据
    HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN , GPIO_PIN_SET);
}

// 轮询状态寄存器 BIT0，直到芯片空闲
void W25Q64_WaitBusy(void) 
{
    uint8_t cmd = 0x05, status;
    do {
        HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN , GPIO_PIN_RESET);
        HAL_SPI_Transmit(&W25Q64_HANDLE, &cmd, 1, 100);            // 发送命令
        HAL_SPI_Receive(&W25Q64_HANDLE,&status,1,100);
        HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN , GPIO_PIN_SET);
    } while (status & 0x01);
}

#endif //W25Q64_MODULE_ENABLED
