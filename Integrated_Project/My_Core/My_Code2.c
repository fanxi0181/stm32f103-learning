//My_Code2.c

#include "My_Code2.h"


#ifdef KEY_DEBOUNCE_MODULE_ENABLED
//初始化按键1实例
key_debounce_t key1 = {
    .port = GPIOA,
    .pin = GPIO_PIN_1,
    .key_debounce_state = key_idle,
    .key_debounce_tick = 0,
    .on_press = NULL
};

//初始化按键2实例
key_debounce_t key2 = {
    .port = GPIOA,
    .pin = GPIO_PIN_2,
    .key_debounce_state = key_idle,
    .key_debounce_tick = 0,
    .on_press = NULL
};

void KEY_DeBounce_State_Machine(key_debounce_t *key)
{
    uint8_t key_level = HAL_GPIO_ReadPin(key->port,key->pin);

    switch (key->key_debounce_state)
    {
    case key_idle:
        {
            if(key_level == GPIO_PIN_RESET)
            {
                key->key_debounce_tick = osKernelGetTickCount();//rate_hz == 1000 一个系统节拍1ms
                key->key_debounce_state = key_press_debounce;
            }
            break;
        }
    case key_press_debounce:
        {
            if(key_level != GPIO_PIN_RESET)
            {
                key->key_debounce_state = key_idle;
            }
            else
            {
                if(osKernelGetTickCount() - key->key_debounce_tick > KEY_DEBOUCNE_TIME_MS)
                {
                    key->key_debounce_state = key_pressed;
                    if(key->on_press) key->on_press();                
                }
            }
            break;
        }
    case key_pressed:
        {
            if(key_level != GPIO_PIN_RESET)
            {
                key->key_debounce_tick = osKernelGetTickCount();
                key->key_debounce_state = key_release_debounce;
            }
            break;
        }
    case key_release_debounce:
        {
            if(key_level == GPIO_PIN_RESET)
            {
                key->key_debounce_state = key_pressed;
            }
            else if(key_level != GPIO_PIN_RESET)
            {
                if(osKernelGetTickCount() - key->key_debounce_tick > KEY_DEBOUCNE_TIME_MS)
                {
                    key->key_debounce_state = key_idle;
                }
            }
            break;      
        }
    }
}

void KEY_SetPressCallback(key_debounce_t *key,key_callback_t callback)
{
    key->on_press = callback;
}

//使用按键回调时 放到最上面初始化
void KEY_DeBounce_Init(void)
{
    KEY_SetPressCallback(&key1,KEY1_Callback);
    KEY_SetPressCallback(&key2,KEY2_Callback);
}

//回调函数 按下按钮发生的操作
void KEY1_Callback(void)
{
    // 示例 如传递keymessage
    // KEYMessage msg = {.w25q64_state = W25Q64_Save_State};
    // osMessageQueuePut(KEY_QueueHandle,&msg,0,0);
}

void KEY2_Callback(void)
{
    // 示例 如传递keymessage
    // KEYMessage msg = {.w25q64_state = W25Q64_State_Recovery};
    // osMessageQueuePut(KEY_QueueHandle,&msg,0,0);
}

#endif /* KEY_DEBOUNCE_MODULE_ENABLED */
