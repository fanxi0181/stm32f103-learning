// sensor_common.h —— 传感器公共定义（PS 光敏 / UR 超声波）
//   仅被传感器模块和 Status_indication_led 引用，其他模块不可见

#ifndef INC_SENSOR_COMMON_H_
#define INC_SENSOR_COMMON_H_

#include "cmsis_os.h"

//—————————— 传感器信号量 & 队列（extern，定义在 freertos.c）——————————
extern osSemaphoreId_t PS_BinarySemHandle;          // 光敏：ADC 注入完成 → 释放
extern osSemaphoreId_t UR_BinarySemHandle;          // 超声波：CC1+CC2 捕获完成 → 释放
extern osMessageQueueId_t Sensor_Notify_QueueHandle; // 传感器数据更新通知（→ PC13 任务）

//—————————— 状态机枚举 ——————————
// 光敏传感器（photosensitive sensor）
typedef enum {
    PS_Init,
    PS_Wait_ADC_JEOC,
    PS_Process,
    PS_Idle
} PS_IJ_State_Machine;

// 超声波测距（ultrasonic ranging）
typedef enum {
    UR_Send_Pulse_to_Trig,
    UR_Wait_Echo,
    UR_Process,
    UR_Idle
} UR_State_Machine;

//—————————— 传感器事件（观察者通知）——————————
typedef enum {
    SENSOR_PS_UPDATED,  // 光敏测量完成
    SENSOR_UR_UPDATED,  // 超声波测量完成
} SensorEvent;

typedef struct {
    SensorEvent event;
} SensorNotifyMsg;

//—————————— 超声波位掩码（输入捕获 CC1/CC2）——————————
typedef enum {
    UR_BIT_CH1 = 0,
    UR_BIT_CH2 = 1,
} Bit_index_enum;

#define BIT_MASK_MACRO(bit)   (1U << (bit))
#define UR_FLAG_CH1_CC1       BIT_MASK_MACRO(UR_BIT_CH1)
#define UR_FLAG_CH2_CC2       BIT_MASK_MACRO(UR_BIT_CH2)

//—————————— 传感器错误码 ——————————
#define UR_ERR_WAIT_ECHO_TIMEOUT      -1.0f  // 超声波：等待回声超时
#define UR_ERR_OUTRANGE               -2.0f  // 超声波：距离超出有效范围（连续 3 次）
#define PS_ERR_Wait_ADC_JEOC_TIMEOUT  -1.0f  // 光敏：ADC 注入转换超时

extern SensorNotifyMsg notify;

#endif // INC_SENSOR_COMMON_H_

