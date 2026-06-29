# 19 配置服务层抽离 & HardFault 底层调试

---

## 一、架构解耦：config_service

之前 W25Q64 模块既管 SPI Flash 底层操作（读写擦），又管配置逻辑（CRC 校验、状态转换）。这次把配置逻辑抽成独立的 `config_service`：

```
之前:  CLI → w25q64（一把抓）
现在:  CLI → config_service → w25q64（分层）
```

| 模块 | 职责 |
|------|------|
| `w25q64.c/h` | SPI Flash 底层驱动（扇区擦写、状态等待） |
| `config_service.c/h` | 配置管理（Load/Save/Erase），CRC 校验，枚举转换，初始化/恢复默认值 |

**好处**：以后换存储介质（比如用内部 EEPROM）只需改 w25q64 底层，config_service 一行不动。

---

## 二、头文件瘦身：sensor_common

把传感器相关的定义（信号量、队列、状态机、错误码、位掩码）从 `app_common.h` 搬到独立的 `sensor_common.h`：

- `PS_IJ_State_Machine` / `UR_State_Machine` 枚举
- `SensorEvent` / `SensorNotifyMsg`
- `PS_BinarySemHandle` / `UR_BinarySemHandle` / `Sensor_Notify_QueueHandle`
- 错误码 `UR_ERR_WAIT_ECHO_TIMEOUT` 等

`app_common.h` 只保留通用类型（LED 状态、队列结构、AppState），传感器定义只有引用了 `sensor_common.h` 的模块才可见。

---

## 三、HardFault 底层调试实战

今天手动制造并分析了 HardFault，理解了 ARM Cortex-M 的故障排查链路：

| 寄存器 | 作用 | 本次实验值 |
|--------|------|-----------|
| `xPSR` | 程序状态，T=0 触发 INVSTATE | T 位为 0 |
| `UFSR` | 用法错误状态 | 0x0002（INVSTATE） |
| `HFSR` | 硬故障状态 | 0x40000000（FORCED） |
| `LR` | 异常返回码，指示 MSP/PSP | 0xFFFFFFF9 / 0xFFFFFFFD |

**调试链**：`BLX R0`(R0=0) → T 被清零 → UFSR 记录 INVSTATE → HFSR 记录 FORCED → 升级为 HardFault → LR 指示栈位置 → 从正确栈中读异常帧

关键理解：FreeRTOS 任务中 LR=`0xFFFFFFFD`（线程+PSP），main 中 LR=`0xFFFFFFF9`（线程+MSP），用错栈指针会读到全零帧。

---

## 四、架构演进对比

| 维度 | 之前 (6.27) | 现在 (6.29) |
|------|-------------|-------------|
| 配置管理 | W25Q64 内嵌 | `config_service` 独立服务层 |
| 传感器定义 | 堆在 `app_common.h` | 独立 `sensor_common.h` |
| 模块数量 | 17 个文件 | 21 个文件 |
| 按键消抖 | 状态机双通道 | 简化为直接 EXTI |
