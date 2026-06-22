// app_config.h —— 应用模块配置文件

#ifndef INC_APP_CONFIG_H_
#define INC_APP_CONFIG_H_

// 模块开关：注释掉即禁用该模块
//#define KEY_DEBOUNCE_MODULE_ENABLED//按键消抖模块是否启用

//此两个文件需同时注释(暂时)
#define SIX_MODULE_LINGKAGE_FREERTOS_ENABLED//是否启用6模块FreeRTOS任务
#define APP_COMMON_ENABLED//是否读取公共枚举 外部声明
#define TEST_CPU_USAGE_ENABLE//是否开启读取cpu使用率任务

//#define TEST_CPU_USAGE_ENABLED//是否开启测试串口DMA
#endif
