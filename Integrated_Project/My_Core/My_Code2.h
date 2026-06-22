//My_Code2.h

#ifndef INC_MY_CODE2_H_
#define INC_MY_CODE2_H_

#include "app_common.h"


#ifdef KEY_DEBOUNCE_MODULE_ENABLED

#define KEY_DEBOUCNE_TIME_MS 20

//按键消抖回调
typedef void(*key_callback_t)(void);

//按键消抖状态机
typedef enum{
    key_idle,//按键空闲
    key_press_debounce,//按下消抖
    key_pressed,//确认按下
    key_release_debounce//释放消抖
}KEY_DeBounce_StateMachine;

//按键消抖状态机封装
typedef struct{
    GPIO_TypeDef *port;
    uint16_t pin;
    KEY_DeBounce_StateMachine key_debounce_state;
    uint32_t key_debounce_tick;//按键消抖状态机专用时间
    key_callback_t on_press;//按下回调1
}key_debounce_t;

//全局按键实例
extern key_debounce_t key1; //按键1
extern key_debounce_t key2; //按键2

//按键消抖确认按下回调函数
void KEY_SetPressCallback(key_debounce_t *key,key_callback_t callback);

//回调函数 按下按钮发生的操作
void KEY1_Callback(void);
void KEY2_Callback(void);

#endif /* KEY_DEBOUNCE_MODULE_ENABLED */

#ifdef KEY_DEBOUNCE_MODULE_ENABLED
    void KEY_DeBounce_Init(void);//初始化按键消抖
    void KEY_DeBounce_State_Machine(key_debounce_t *key);//按键消抖状态机函数
#else
    #define KEY_DeBounce_Init()
    #define KEY_DeBounce_State_Machine(key)
#endif

#endif /* INC_MY_CODE2_H_ */
