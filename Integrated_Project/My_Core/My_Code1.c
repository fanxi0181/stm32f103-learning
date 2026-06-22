//My_Code1.c

#include "My_Code1.h"
#include "My_Code3.h"


#ifdef SIX_MODULE_LINGKAGE_FREERTOS_ENABLED

uint8_t rxData;
uint32_t current_time = 0;
KEYMessage keymessage;
USARTMessage usartmessage;

//————————————无用全局变量 留痕 开始————————————
// volatile LED_State g_state.led = LED_State_OFF;
// W25Q64_State current_w25q64_mode;
// volatile OLED_State g_state.oled = OLED_State_Display3;
//————————————无用全局变量 留痕 结束————————————

//APP全局当前状态初始值
volatile AppState g_state = {
    .led = LED_State_OFF,
    .oled = OLED_State_Display3,
    .w25q64 = W25Q64_State_Save
};


void vIP_USARTTask(void *argument)
{
    UART1_Receive_Start();
    char cmdbuff[8];
    uint16_t index = 0;
    uint8_t byte;
    for(;;)
    {
        if(osSemaphoreAcquire(USART_CountingSemHandle,osWaitForever) == osOK)
        {

            byte = rxData;
            if(byte == '\r' || byte == '\n')
            {
                if(index > 0)
                {
                    cmdbuff[index] = '\0';
                    if(strcmp(cmdbuff,"1") == 0)
                    {
                        usartmessage.led_state = LED_State_ON;
                        usartmessage.oled_state = OLED_State_Display1;
                    }
                    else if(strcmp(cmdbuff,"2") == 0)
                    {
                        usartmessage.led_state = LED_State_OFF;
                        usartmessage.oled_state = OLED_State_Display2;                        
                    }
                    else if((strcmp(cmdbuff,"3") == 0))
                    {
                        usartmessage.led_state = LED_State_Flicker;
                        usartmessage.oled_state = OLED_State_Display3;//恢复开始界面                       
                    }
                    else
                    {
                        UART_DMA_Printf("Invalid_input\r\n");
                    }
                    if(osMessageQueuePut(USART_QueueHandle,&usartmessage,0,pdMS_TO_TICKS(100)) != osOK)
                    {
                        UART_DMA_Printf("usartQueue_err\r\n");
                    }
                    index = 0;
                    memset(cmdbuff,0,sizeof(cmdbuff));
                }
            }
            else
            {
                if(index < (sizeof(cmdbuff) - 1))
                {
                    cmdbuff[index++] = byte;
                }
                else
                {
                    index = 0;
                    memset(cmdbuff,0,sizeof(cmdbuff));
                }
            }
            UART1_Receive_Start();//接收完成再次开启中断，准备接收
        }
    }
}

void UART1_Receive_Start(void)
{
    HAL_UART_Receive_IT(&huart1,&rxData,1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart -> Instance == USART1)
    {
        osSemaphoreRelease(USART_CountingSemHandle);
    }
}

void vIP_LEDTask(void *argument)
{
    static uint8_t led_flag = 0;
    OLED_State last_oled_mode = OLED_State_Display3;//上一次状态
    for(;;)
    {
        USARTMessage msg;
        bool new_msg = false;

        if(osMessageQueueGet(USART_QueueHandle,&msg,0,0) == osOK)
        {
            g_state.led = msg.led_state;
            g_state.oled = msg.oled_state;
            new_msg = true;
            led_flag = 0;
        }

        //第一是串口信号 第二是w25q64信号 此处统一改变
        if(new_msg || (g_state.oled != last_oled_mode))
        {
            osMutexAcquire(General_MutexHandle,osWaitForever);
            OLED_Clear();
            switch (g_state.oled)
            {
            case OLED_State_Display1:
                OLED_ShowString(0, 0, "LED: ON", OLED_8X16);
                break;
            case OLED_State_Display2:
                OLED_ShowString(0, 0, "LED: OFF", OLED_8X16);
                break;
            case OLED_State_Display3:
                OLED_ShowString(0, 0, "STM32: OK", OLED_8X16);
                break;           
            default:
                OLED_ShowString(0, 0, "STM32: OK", OLED_8X16);//默认显示开机界面
                break;
            }
            OLED_Update();
            osMutexRelease(General_MutexHandle);

            last_oled_mode = g_state.oled;
        }

        switch (g_state.led)
        {
        case LED_State_ON:
            {
                HAL_GPIO_WritePin(PA3_LED1_GPIO_Port,PA3_LED1_Pin,GPIO_PIN_SET);
                break;
            }
        case LED_State_OFF:
            {
                HAL_GPIO_WritePin(PA3_LED1_GPIO_Port,PA3_LED1_Pin,GPIO_PIN_RESET);
                break;
            }
        case LED_State_Flicker:
            {
                if(osKernelGetTickCount() - current_time >= pdMS_TO_TICKS(500))
                {
                    current_time = osKernelGetTickCount();
                    led_flag = !led_flag;
                    HAL_GPIO_WritePin(PA3_LED1_GPIO_Port,PA3_LED1_Pin,led_flag ? GPIO_PIN_SET : GPIO_PIN_RESET);
                }
                break;
            }      
        default:
            {
                UART_DMA_Printf("ledtask_err\r\n");
                break;
            }
        }
        osDelay(pdMS_TO_TICKS(10));
    }
}

void vIP_PA11Flick(void *argument)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(PA11_LED2_GPIO_Port,PA11_LED2_Pin);
        osDelay(500);
    }
}

void vIP_OLEDTask(void *argument)
{
    //Init_OLED();
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, "STM32: OK", OLED_8X16);//上电默认显示
    OLED_Update();
    for(;;)
    { 
        osDelay(pdMS_TO_TICKS(100));
    }
}

void Init_OLED(void)
{
    uint8_t commands[] = {0x00,0x8d,0x14,0xaf,0xa5};
    HAL_I2C_Master_Transmit(&hi2c1,0x78,commands,sizeof(commands)/sizeof(commands[0]),HAL_MAX_DELAY);
    uint8_t rcvd;
    HAL_I2C_Master_Receive(&hi2c1,0x78,&rcvd,sizeof(rcvd),HAL_MAX_DELAY);
    if((rcvd & (0x01 << 6)) == 0)
    {
        UART_DMA_Printf("OLED_OK\r\n");        
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if((GPIO_Pin == PA1_KEY1_Pin) || (GPIO_Pin == PA2_KEY2_Pin))
    { 
        osSemaphoreRelease(KEY_BinarySemHandle);
    }
}

void vIP_KEYTask(void *argument)
{
    for(;;)
    {
        KEYMessage msg;
        osSemaphoreAcquire(KEY_BinarySemHandle,osWaitForever);
        vTaskDelay(pdMS_TO_TICKS(20));
        if(HAL_GPIO_ReadPin(PA1_KEY1_GPIO_Port,PA1_KEY1_Pin) == GPIO_PIN_RESET)
        {
            msg.w25q64_state = W25Q64_State_Save;
        }
        else if(HAL_GPIO_ReadPin(PA2_KEY2_GPIO_Port,PA2_KEY2_Pin) == GPIO_PIN_RESET)
        {
            msg.w25q64_state = W25Q64_State_Recovery;
        }
        else
            continue;

        if(osMessageQueuePut(KEY_QueueHandle,&msg,0,0) != osOK)
        {
            UART_DMA_Printf("Queue_Full\r\n");
        }
    }
}

void vIP_W25Q64Task(void *argument)
{
    static uint8_t LEDON = 0x01;
    static uint8_t LEDOFF = 0x00;
    static uint8_t ledmode;
    static uint8_t rcvy;
    for(;;)
    {
        KEYMessage msg;
        if(osMessageQueueGet(KEY_QueueHandle,&msg,0,osWaitForever) == osOK)
        {
            ledmode = HAL_GPIO_ReadPin(PA3_LED1_GPIO_Port,PA3_LED1_Pin);
            if(msg.w25q64_state == W25Q64_State_Save)
            {
                if(ledmode == GPIO_PIN_SET)
                {
                    W25Q64_Save_State(LEDON);
                }
                else if(ledmode == GPIO_PIN_RESET)
                {
                    W25Q64_Save_State(LEDOFF);
                }
                UART_DMA_Printf("save_sucess\r\n");              
            }
            else if(msg.w25q64_state == W25Q64_State_Recovery)
            {
                rcvy = W25Q64_Read_ME();
                if(rcvy == LEDON)//更新led和oled状态 通过全局变量 跳到led任务
                {
                    g_state.led = LED_State_ON;
                    g_state.oled = OLED_State_Display1;
                }
                else if(rcvy == LEDOFF)
                {
                    g_state.led = LED_State_OFF;
                    g_state.oled = OLED_State_Display2;                
                }
                else
                {
                    g_state.oled = OLED_State_Display3;
                }
                UART_DMA_Printf("rcvy_sucess\r\n");                    
            }
        }
        osDelay(pdMS_TO_TICKS(10));       
    }
}

void W25Q64_Save_State(uint8_t State)
{
    uint8_t Buffer[6];
    Buffer[0] = 0x06;
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port,PA4_W25Q64_Pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1,Buffer,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port,PA4_W25Q64_Pin,GPIO_PIN_SET);

    Buffer[0] = 0x20;
	Buffer[1] = 0x00;
	Buffer[2] = 0x00;
	Buffer[3] = 0x00;
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port,PA4_W25Q64_Pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1,Buffer,4,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port,PA4_W25Q64_Pin,GPIO_PIN_SET);
    
    W25Q64_WaitBusy(); 

    Buffer[0] = 0x06;
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port,PA4_W25Q64_Pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1,Buffer,1,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port,PA4_W25Q64_Pin,GPIO_PIN_SET);

	Buffer[0] = 0x02;
	Buffer[1] = 0x00;
	Buffer[2] = 0x00;
	Buffer[3] = 0x00;
	Buffer[4] = State;
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port,PA4_W25Q64_Pin,GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1,Buffer,5,HAL_MAX_DELAY);
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port,PA4_W25Q64_Pin,GPIO_PIN_SET);
    
    W25Q64_WaitBusy(); 

    osDelay(pdMS_TO_TICKS(5));
}

uint8_t W25Q64_Read_ME(void)
{
    uint8_t Buffer[5];
    W25Q64_WaitBusy(); 
    Buffer[0] = 0x03;
    Buffer[1] = 0x00;
    Buffer[2] = 0x00;
    Buffer[3] = 0x00;
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port, PA4_W25Q64_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, Buffer, 4, HAL_MAX_DELAY);   // 发送读命令
    HAL_SPI_Receive(&hspi1, Buffer, 1, HAL_MAX_DELAY);    // HAL会自动发OxFF接收数据
    HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port, PA4_W25Q64_Pin, GPIO_PIN_SET);
    return Buffer[0];
}

void W25Q64_WaitBusy(void) {
    uint8_t cmd = 0x05, status;
    do {
        HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port, PA4_W25Q64_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);            // 发送命令
        HAL_SPI_Receive(&hspi1,&status,1,100);
        HAL_GPIO_WritePin(PA4_W25Q64_GPIO_Port, PA4_W25Q64_Pin, GPIO_PIN_SET);
    } while (status & 0x01);
}

#endif //SIX_MODULE_LINGKAGE_FREERTOS
