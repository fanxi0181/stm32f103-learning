//config_service.c  将保存和恢复计算crc从 w25q64模块中解耦 w25q64文件只进行flash操作

#include "config_service.h"
#include "w25q64.h"       // 只调用纯 Flash API
#include "uart_dma.h"

#ifdef CONFIG_SERVICE_ENABLED

static uint32_t Config_CRC(const W25Q64_Config *cfg)
{
    return  (uint32_t)cfg->led_state + 
            (uint32_t)(cfg->voltage_threshold * 1000.f) +
            (uint32_t)(cfg->distance_threshold * 1000.f);
}

void Config_Load(void)
{
    W25Q64_Config cfg;
    W25Q64_Read_All(&cfg);

    uint32_t calc_crc = Config_CRC(&cfg);
    if(cfg.crc == calc_crc)
    {
        //恢复全局状态
        g_state.led = Store_to_LED_State((StoredLedState)cfg.led_state);
        g_state.voltage_threshold = cfg.voltage_threshold;
        g_state.distance_threshold = cfg.distance_threshold;

        UART_DMA_Printf("Restored: LED=%s, Vthr=%.2fV, Dthr=%.2fm\r\n",
                        (g_state.led == LED_State_ON) ? "ON" : "OFF",
                        g_state.voltage_threshold, g_state.distance_threshold);

        //通知OLED刷新
        OLEDDisplayRequest oled_req = {
            .oled_state = (g_state.led == LED_State_ON)
            ? OLED_State_Display1 : OLED_State_Display2,
            .force_refresh = true
        };
        osMessageQueuePut(OLED_Display_QueueHandle,&oled_req,0,0);
    }
    else
    {
        UART_DMA_Printf("Flash_config_invalid,using_defaults.\r\n");
    }
}

void Config_Save(void)
{
    W25Q64_Config cfg;
    cfg.led_state = (uint8_t)LED_State_to_Store(g_state.led);
    cfg.voltage_threshold = g_state.voltage_threshold;
    cfg.distance_threshold = g_state.distance_threshold;
    cfg.crc = Config_CRC(&cfg);

    W25Q64_Save_All(&cfg);

    UART_DMA_Printf("Saved: LED=%s, Vthr=%.2fV, Dthr=%.2fm\r\n",
                    (g_state.led == LED_State_ON) ? "ON" : "OFF",
                    g_state.voltage_threshold, g_state.distance_threshold);
}

bool Config_Erase(void)
{
    // 擦除 Flash 配置扇区 → 恢复 g_state 默认值 → 通知 OLED
    bool ok = W25Q64_Erase_Config_Sector();   // ← 纯 Flash 擦

    g_state.led = LED_State_OFF;
    g_state.voltage_threshold = 1.5f;
    g_state.distance_threshold = 0.1f;

    OLEDDisplayRequest oled_req = { OLED_State_Display3, true };
    osMessageQueuePut(OLED_Display_QueueHandle, &oled_req, 0, 0);

    return ok;
}


#endif //CONFIG_SERVICE_ENABLED
