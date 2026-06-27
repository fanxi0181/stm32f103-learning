# 01 按键控制 LED — GPIO 入门

> **芯片**：STM32F103C8T6 | **环境**：VSCode + Keil + Keil Assistant  
> **主题**：GPIO 初始化、八种模式、位运算、指针/地址辨析、闭卷默写验证

---

## 一、核心代码

### 按键控制 LED（按键按下亮，松开灭）

```c
void key_process_led(void)
{
    // 1. 开启 GPIOA 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_StructInit = {0};

    // 2. 配置 LED 引脚 PA0 → 推挽输出
    GPIO_StructInit.GPIO_Pin   = GPIO_Pin_0;
    GPIO_StructInit.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_StructInit.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_StructInit);

    // 3. 配置按键引脚 PA1 → 上拉输入（不同模式需分开初始化！）
    GPIO_StructInit.GPIO_Pin   = GPIO_Pin_1;
    GPIO_StructInit.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_StructInit.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_StructInit);

    while (1)
    {
        // 上拉输入：未按为 SET，按下为 RESET
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == RESET)
        {
            Delay(20);  // 消抖
            GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);   // 高电平点亮
        }
        else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == SET)
        {
            Delay(20);
            GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET); // 低电平熄灭
        }
    }
}
```

---

## 二、核心知识点

### 1. GPIO 初始化铁三角

| 步骤 | 操作 | 说明 |
|------|------|------|
| ① 开时钟 | `RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE)` | 任何外设第一步 |
| ② 配结构体 | `GPIO_InitTypeDef` → Pin / Speed / Mode | 不同模式必须分开 Init |
| ③ 调初始化 | `GPIO_Init(GPIOx, &GPIO_InitStruct)` | 传入结构体指针 |

### 2. GPIO 八种模式速查

| 模式 | 英文全称 | 用途 |
|------|----------|------|
| **Out_PP** | Output Push-Pull | LED 驱动，直接高低电平 |
| **Out_OD** | Output Open-Drain | I2C 等总线（需外部上拉） |
| **AF_PP** | Alternate Function Push-Pull | 串口、SPI 等复用输出 |
| **AF_OD** | Alternate Function Open-Drain | I2C、CAN 等复用开漏 |
| **IPU** | Input Pull-Up | 按键检测（空闲高，按下低） |
| **IPD** | Input Pull-Down | 按键检测（空闲低，按下高） |
| IN_FLOATING | Input Floating | 浮空输入 |
| AIN | Analog Input | ADC 模拟输入 |

### 3. 时钟系统命名拆解

| 缩写 | 全称 | 含义 |
|------|------|------|
| RCC | Reset and Clock Control | 复位与时钟控制 |
| APB2 | Advanced Peripheral Bus 2 | 高速总线 72MHz（挂 GPIOA/B/C） |
| Periph | Peripheral | 外设 |

### 4. 指针「房间-门牌-纸条」模型

- **变量** = 房间（存数据）  
- **地址** = 门牌号（`&a`）  
- **指针** = 记有门牌号的纸条（`int *p = &a;`）
- `*` 定义时表示指针类型，使用时表示解引用（去该地址取数据）
- 函数传参改外部变量 → 传指针；参数已是指针 → 直接传，不加 `&`

```c
void inputTwoNumbers(int* x, int* y) {
    scanf("%d%d", x, y);  // x 本身已是地址，不需要 &x
}
int a, b;
inputTwoNumbers(&a, &b);  // 取 a、b 的地址传入
```

### 5. 位运算核心规则

| 运算符 | 规则 | 典型用法 |
|--------|------|----------|
| `&`（按位与） | 全 1 才 1 | 清指定位：`0xAF & 0xFE = 0xAE` |
| `\|`（按位或） | 有 1 则 1 | 置指定位：`0x06 \| 0x01 = 0x07` |
| `<<` | 左移 n 位 = × 2ⁿ | `0x01 << 5 = 0x20` |
| `>>` | 右移 n 位 = ÷ 2ⁿ | `0x80 >> 3 = 0x10` |

> **易错**：混淆 `&&`（逻辑与）和 `&`（按位与）；十六进制每位是 4 bit（0xA = 1010 而非 0011）

### 6. 按键消抖逻辑

1. 读按键 → 若按下 → `Delay(20ms)` → 再读一次
2. 仍为按下 → 确认动作（开/关 LED）
3. 释放后也延时，避免重复触发

---

## 三、自测记录

| 题目 | 错误答案 | 正确答案 | 原因 |
|------|----------|----------|------|
| `0xAF & 0xFE` | 0xAF | **0xAE** | 误以为 F 是最后一位 |
| `0x06 \| 0x01` | 0x00 | **0x07** | 把按位或当成了按位与 |
| `0x55 \| 0xAA` | 0xBB | **0xFF** | 把 A 当成 0011（实际 1010） |
| `0x01<<7 \| 0x01<<3` | 0x90 | **0x88** | 右移多进一位 |

---

## 四、心得

- 记忆英文全称（RCC/APB2/GPIO/IPU 等）能大幅提升代码可读性
- 不同模式的引脚 **不能共用同一个结构体一次性初始化**，必须分别赋值后调用 `GPIO_Init`
- 闭卷默写 + 烧录验证是检验真正掌握的唯一标准