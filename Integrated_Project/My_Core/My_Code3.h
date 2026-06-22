//My_Code3.h

#ifndef INC_MY_CODE3_H_
#define INC_MY_CODE3_H_

#include "app_common.h"

#ifdef HAL_UART_MODULE_ENABLED
    #include "usart.h"
    #define ADD_DMA_USART huart1
    #define TX_DMA_BUFFER_SIZE 64
    #define RX_DMA_BUFFER_SIZE 64
    #define UART_DMA_Printf(...) do{\
        int len = sprintf((char *)tx_dma_buffer,__VA_ARGS__);\
        HAL_UART_Transmit_DMA(&ADD_DMA_USART,tx_dma_buffer,len);\
    }while(0)

    extern uint8_t tx_dma_buffer[TX_DMA_BUFFER_SIZE];
    extern uint8_t rx_dma_buffer[RX_DMA_BUFFER_SIZE];
    extern uint8_t RX_RCV_Flish_Flag;

    int fputc(int ch,FILE *f);
    int fgetc(FILE *f);
#endif

#ifdef TEST_CPU_USAGE_ENABLED
    void T_CPU_U_Task(void *argument);
    void My_TIM3_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#else
    #define T_CPU_U_Task(argument)
    #define My_TIM3_PeriodElapsedCallback(htim)
#endif //TEST_CPU_USAGE_ENABLED

#endif /* INC_MY_CODE3_H_ */

