//My_Code3.c

#include "My_Code3.h"

#if defined(HAL_UART_MODULE_ENABLED)
    uint8_t tx_dma_buffer[TX_DMA_BUFFER_SIZE] = {0};
    uint8_t rx_dma_buffer[RX_DMA_BUFFER_SIZE] = {0};
    uint8_t RX_RCV_Flish_Flag = 0;
    
    void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
    {
        if(huart == &ADD_DMA_USART)
        {
            if(RX_RCV_Flish_Flag == 0)
            {
                RX_RCV_Flish_Flag = 1;
                if(Size < RX_DMA_BUFFER_SIZE)
                {
                    rx_dma_buffer[Size] = '\0';
                }
                HAL_UARTEx_ReceiveToIdle_DMA(&ADD_DMA_USART,rx_dma_buffer,RX_DMA_BUFFER_SIZE);
            }
        }
    }

    int fputc(int ch,FILE *f)
    {
        HAL_UART_Transmit(&ADD_DMA_USART,(uint8_t *)&ch,1,100);
        return ch;
    }

    int fgetc(FILE *f)
    {
        uint8_t ch;
        HAL_UART_Receive(&ADD_DMA_USART,&ch,1,100);
        return ch;
    }
#endif

#ifdef TEST_CPU_USAGE_ENABLED
uint8_t tim3_finish_flag = 0;

char uart1_tx_buf[64];//RAM缓冲区
float cpu_use = 0;//CPU使用率
float dt = 0;

float kp = 0;
float ki = 0;
float kd = 0;

void T_CPU_U_Task(void *argument)
{
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_UARTEx_ReceiveToIdle_DMA(&ADD_DMA_USART,rx_dma_buffer,RX_DMA_BUFFER_SIZE);//首先开启回调中断
    for(;;)
    {
        if(tim3_finish_flag)
        {
            uint32_t start = __HAL_TIM_GetCounter(&htim3);

            tim3_finish_flag = 0;

            // int len = sprintf(uart1_tx_buf,"%.2f\n",cpu_use);
            // HAL_UART_Transmit(&ADD_DMA_USART,(uint8_t *)uart1_tx_buf,len,100);
            //printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",cpu_use,sinf(dt),2*sinf(dt),4*sinf(dt),8*sinf(dt),16*sinf(dt));
            //UART_DMA_Printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",cpu_use,sinf(dt),2*sinf(dt),4*sinf(dt),8*sinf(dt),16*sinf(dt));
            if(RX_RCV_Flish_Flag)
            {
                RX_RCV_Flish_Flag = 0;
                UART_DMA_Printf("%s",rx_dma_buffer);
            }
            dt+=0.1f;
            cpu_use = (float)((__HAL_TIM_GetCounter(&htim3) - start)/5000.f) * 100;
        }
    }
}

void My_TIM3_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        tim3_finish_flag = 1;
    }
}
#endif //TEST_CPU_USAGE_ENABLED

