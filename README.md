# STM32F103C8T6 嵌入式开发学习记录

> **环境**：VSCode + Keil + Keil Assistant → CubeMX + FreeRTOS  
> **库文件**：B站 [铁头山羊](https://space.bilibili.com/471714251) 标准外设库 / HAL 库  
> **方法**：闭卷默写 → 烧录验证 → 错误复盘 → 重构优化

---

## 📁 项目源码

| 项目 | 说明 | 技术栈 |
|------|------|--------|
| `01-BareMetal-Practice/` | 裸机综合练习（GPIO→ADC 全部外设） | 标准外设库 + Keil |
| `02-Timer-Exercises/` | 定时器专项（PWM/输入捕获/主从模式） | 标准外设库 + Keil |
| `Integrated_Project/` | 第一周综合项目 FreeRTOS 重构 | CubeMX + HAL + FreeRTOS |

> ⚠️ 每日练习代码分散在 `user/main.c` 和 `my_lib/my_code.c / my_code2.c` 中，函数声明较乱，后续统一整理。

---

## 📖 学习笔记（按日期）

| 日期 | 主题 | 文档 |
|------|------|------|
| 6.01 | GPIO 按键控制 LED | [docs/6.01-按键控制LED.md](docs/6.01-按键控制LED.md) |
| 6.02 | USART 串口通信 | [docs/6.02-USART串口通信.md](docs/6.02-USART串口通信.md) |
| 6.03 | I2C 通信 | [docs/6.03-I2C通信.md](docs/6.03-I2C通信.md) |
| 6.04 | SPI + W25Q64 Flash | [docs/6.04-SPI通信Flash.md](docs/6.04-SPI通信Flash.md) |
| 6.05 | NVIC + EXTI 中断 | [docs/6.05-中断系统NVIC+EXTI.md](docs/6.05-中断系统NVIC+EXTI.md) |
| 6.06 | 时钟树 + 第一周综合项目 | [docs/6.06-时钟树+综合项目.md](docs/6.06-时钟树+综合项目.md) |
| 6.07 | 定时器 + PWM 呼吸灯 | [docs/6.07-定时器PWM呼吸灯.md](docs/6.07-定时器PWM呼吸灯.md) |
| 6.09 | 输入捕获 + 超声波测距 | [docs/6.09-输入捕获超声波测距.md](docs/6.09-输入捕获超声波测距.md) |
| 6.10 | 主从模式 + PWM 测量 | [docs/6.10-主从模式PWM测量.md](docs/6.10-主从模式PWM测量.md) |
| 6.11 | ADC 光敏传感器 + 回调函数 | [docs/6.11-ADC光敏传感器.md](docs/6.11-ADC光敏传感器.md) |
| 6.14 | ADC 注入序列 + 扫描模式 | [docs/6.14-ADC注入序列+扫描模式.md](docs/6.14-ADC注入序列+扫描模式.md) |
| 6.15 | FreeRTOS 入门 | [docs/6.15-FreeRTOS入门.md](docs/6.15-FreeRTOS入门.md) |
| 6.16 | FreeRTOS 队列实战 | [docs/6.16-FreeRTOS队列实战.md](docs/6.16-FreeRTOS队列实战.md) |
| 6.17 | 二进制信号量 | [docs/6.17-二进制信号量.md](docs/6.17-二进制信号量.md) |
| 6.18 | 综合项目 RTOS 重构 | [docs/6.18-综合项目RTOS重构.md](docs/6.18-综合项目RTOS重构.md) |
| 6.19 | 按键消抖模块化封装 | [docs/6.19-按键消抖模块化.md](docs/6.19-按键消抖模块化.md) |

---

## 📚 心得分享

- [外设中英文对照表](心得分享/外设中英文对照表.md) — 13 大类，180+ 术语

---

## 🛠 外设覆盖

- [x] GPIO（推挽/开漏/上拉/下拉）
- [x] USART（重映射、中断接收、超时保护）
- [x] I2C（主机收发、ACK/NAK、时钟拉伸）
- [x] SPI（全双工、W25Q64 Flash、借时钟）
- [x] NVIC + EXTI（优先级分组、外部中断）
- [x] TIM（时基/PWM/输入捕获/主从模式）
- [x] ADC（常规/注入/扫描/定时器触发）
- [x] OLED（I2C 驱动、回调适配）
- [x] FreeRTOS（任务、队列、信号量、互斥锁）