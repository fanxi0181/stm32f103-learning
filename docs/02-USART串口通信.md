# 6.2 USART 串口通信

> **芯片**：STM32F103C8T6 | **环境**：VSCode + Keil + Keil Assistant  
> **主题**：USART 帧格式、波特率计算、重映射、超时保护、fputc 重定向、Doxygen 注释

---

## 一、核心代码

### 1. 初始化（重映射 + USART）

```c
void init_ledpc13_usartremap(void)
{
    // PC13 板载 LED
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_StructInit = {0};
    GPIO_StructInit.GPIO_Pin   = GPIO_Pin_13;
    GPIO_StructInit.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_StructInit.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOC, &GPIO_StructInit);

    // 重映射 USART1: PA9/PA10 → PB6/PB7
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 必须先开 AFIO 时钟
    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);       // 参数是宏，不是 GPIO 端口名

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // TX: PB6 → 推挽复用输出，速度 10MHz 保 115200 稳定
    GPIO_StructInit.GPIO_Pin   = GPIO_Pin_6;
    GPIO_StructInit.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_StructInit.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_StructInit);

    // RX: PB7 → 上拉输入
    GPIO_StructInit.GPIO_Pin  = GPIO_Pin_7;
    GPIO_StructInit.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_StructInit);

    // USART 参数
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_InitTypeDef USART_StructInit = {0};
    USART_StructInit.USART_BaudRate   = 115200;
    USART_StructInit.USART_WordLength = USART_WordLength_8b;
    USART_StructInit.USART_Parity     = USART_Parity_No;
    USART_StructInit.USART_StopBits   = USART_StopBits_1;
    USART_StructInit.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_StructInit);
    USART_Cmd(USART1, ENABLE);
}
```

### 2. 带超时的发送函数

```c
int send_string_usart(USART_TypeDef *USARTx, uint8_t *pData,
                      uint16_t Size, int32_t TimeOut)
{
    if (pData == NULL || Size == 0)
        return USART_PARAM_ERR;

    uint32_t Start_Time = GetTick();

    for (uint16_t i = 0; i < Size; i++)
    {
        // 等待 TXE（发送寄存器空）
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
        {
            if (TimeOut != -1 && (GetTick() - Start_Time) > TimeOut)
                return USART_TIMEOUT;
        }
        USART_SendData(USARTx, pData[i]);
    }

    // 等待 TC（移位寄存器完全发出）
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
    {
        if (TimeOut != -1 && (GetTick() - Start_Time) > TimeOut)
            return USART_TIMEOUT;
    }
    USART_ClearFlag(USARTx, USART_FLAG_TC);  // TC 必须手动清
    return USART_OK;
}
```

### 3. 带超时的接收函数

```c
uint16_t recv_string_usart(USART_TypeDef *USARTx, uint8_t *outpData,
                            uint16_t Size, int32_t TimeOut)
{
    uint16_t i = 0;
    uint32_t Start_Time = GetTick();

    do {
        if (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET) {
            outpData[i++] = USART_ReceiveData(USARTx);
            if (i == Size) break;
        }
    } while (TimeOut < 0 || (GetTick() - Start_Time) <= TimeOut);

    return i;  // 返回实际收到字节数
}
```

### 4. fputc 重定向（printf 输出到串口）

```c
int fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint8_t)ch);  // ch 是 int，只取低 8 位
    return ch;
}
```

### 5. 主流程：串口命令控制 LED + 按键反馈

```c
void process_usart_send_and_recieve(void)
{
    init_ledpc13_usartremap();
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);  // 上电亮灯

    while (1)
    {
        uint8_t ch;
        uint16_t ret = recv_string_usart(USART1, &ch, 1, 50);

        if (ret == 0)  // 超时没收到数据 → 检查按键
        {
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == RESET)
            {
                Delay(20);
                if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == RESET)
                {
                    send_string_usart(USART1, "Button Pressed!", 15, 200);
                    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == RESET);
                    Delay(20);
                }
            }
        }
        else  // 收到数据
        {
            if (ch == '1')
            {
                GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);  // 灭灯
                send_string_usart(USART1, "LED ON", 6, 200);
            }
            else if (ch == '0')
            {
                GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);    // 亮灯
                send_string_usart(USART1, "LED OFF", 7, 200);
            }
        }
    }
}
```

---

## 二、核心知识点

### 1. USART 帧格式

```
空闲 → [起始位 0] [D0 D1 D2 D3 D4 D5 D6 D7] [校验位?] [停止位 1/1.5/2] → 空闲
         LSB 先发                                        高电平
```

常用配置：**8 数据位、无校验、1 停止位**

### 2. 波特率计算（BRR 寄存器）

```
USARTDIV = f_CLK / (16 × 波特率)
         = 72MHz / (16 × 115200)
         = 39.0625
```

BRR 编码：(39 << 4) | 1 = 0x271
- 整数部分左移 4 位，小数部分 = 1/16 → 存为 1

### 3. 引脚模式选择

| 引脚 | 模式 | 原因 |
|------|------|------|
| **TX** | AF_PP（复用推挽） | 高速强驱动，10MHz 以上 |
| **RX** | IPU（上拉输入） | 空闲稳定高电平，避免误触发起始位 |

### 4. 重映射三步

```c
① RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 必开 AFIO
② GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);       // 参数是 Remap 宏！
③ 按新引脚配置 GPIO
```

### 5. 三大标志位

| 标志 | 含义 | 清除方式 |
|------|------|----------|
| **TXE** | 发送数据寄存器空 | 写 `USART_SendData()` 自动清 |
| **TC** | 移位寄存器发送完毕 | **必须手动** `USART_ClearFlag()` |
| **RXNE** | 接收缓冲区非空 | 读 `USART_ReceiveData()` 自动清 |

> TXE 表示可以写下一字节，TC 才表示总线完全空闲。发送后应等 TC。

### 6. 超时保护模式

```c
int32_t TimeOut;  // -1 = 无限等待, N = N 毫秒后超时
uint32_t Start_Time = GetTick();

while (等待条件) {
    if (TimeOut != -1 && (GetTick() - Start_Time) > TimeOut)
        return USART_TIMEOUT;  // 超时判断必须在循环内部！
}
```

### 7. Doxygen 注释规范

```c
/**
 * @brief  带超时的串口发送
 * @param  USARTx  串口号
 * @param  pData   数据数组
 * @param  Size    字节数
 * @param  TimeOut 超时毫秒，-1 表示无限等待
 * @retval 0 成功，1 超时，2 参数错误
 * @note   发送后等待 TC 标志确保数据完全发出
 */
```

### 8. 调试要点 — 本次踩坑记录

| 错误 | 原因 | 修复 |
|------|------|------|
| 重映射不生效 | `GPIO_PinRemapConfig(GPIOA, ...)` 参数写错 | 改为 `GPIO_Remap_USART1` |
| 编译报错重复定义 | 同一函数内多次 `GPIO_InitTypeDef xxx = {0}` | 只定义一次，后面直接赋值 |
| TX 发送不稳定 | PB6 Speed 只设了 2MHz | 改为 10MHz |
| 超时不生效 | 超时判断写在 while 外面 | 移到 while 循环内部 |
| 按钮无响应 | 忘了初始化 PA1 的 GPIO | 调用 `init_buttonandled()` |
| 无数据时不停发消息 | 收到 0 字节 vs 收到字符没区分 | 用 `ret == 0` 判断加 `else` |

---

## 三、心得

- USART 驱动的核心是理解 **帧格式 → 波特率 → 引脚模式 → 标志位** 这条链路
- 超时保护不是"锦上添花"，是裸机通信的**必需品**，-1 表示无限等待这个设计很实用
- TC 标志容易和 TXE 混淆：TXE 让你发下一字节，TC 告诉你总线已空闲
- 重映射要记得开 AFIO 时钟，参数是 Remap 宏不是 GPIO 端口名