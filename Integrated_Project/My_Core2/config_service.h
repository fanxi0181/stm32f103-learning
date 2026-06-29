//config_service.H  

#ifndef INC_CONFIG_SERVICE_H_
#define INC_CONFIG_SERVICE_H_

#include "app_common.h"

#ifdef CONFIG_SERVICE_ENABLED

/* 存入 Flash 的配置结构体 */
typedef struct {
    uint8_t led_state;           // 存储的 LED 状态（见下方枚举）
    float   voltage_threshold;   // 电压阈值（V）
    float   distance_threshold;  // 距离阈值（m）
    uint32_t crc;                // 简易校验和
} W25Q64_Config;

/* 存储用的 LED 状态值 */
typedef enum {
    STORE_LED_OFF = 0x00,
    STORE_LED_ON  = 0x01
} StoredLedState;

/* 从 AppState 的 LED 枚举转到存储值 */
static inline StoredLedState LED_State_to_Store(LED_State state) {
    return (state == LED_State_ON) ? STORE_LED_ON : STORE_LED_OFF;
}

/* 从存储值转到 AppState 的 LED 枚举 */
static inline LED_State Store_to_LED_State(StoredLedState stored) {
    return (stored == STORE_LED_ON) ? LED_State_ON : LED_State_OFF;
}

/* 配置服务接口 */
void Config_Load(void);    // 从 Flash 读 → 校验 → 恢复 g_state → 通知 OLED
void Config_Save(void);    // 从 g_state 读 → 写入 Flash
bool Config_Erase(void);   // 擦除 Flash 配置扇区

#else
#define Config_Load()
#define Config_Save()
#define Config_Erase()  false

#endif //CONFIG_SERVICE_ENABLED

#endif //INC_CONFIG_SERVICE_H_
