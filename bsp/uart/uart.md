# UART 模块使用说明

## 概述

UART 模块基于**句柄（Handle）注册机制**，通过结构体封装 UART 外设的全部配置和运行时状态，支持同时使用最多 4 个 UART 实例（MSPM0G350X 的 UART0~UART3）。

核心设计：

- `UART_Config` — 初始化配置，指定硬件资源
- `UART_Handle` — 运行时句柄，封装硬件引用 + 全部状态
- `UART_Init()` — 注册函数，返回句柄指针
- 所有操作函数接受句柄指针，**无全局变量**

## 快速开始

### 1. 注册 UART 实例

```c
#include "uart.h"

UART_Handle *uart_print;

void main(void) {
    SYSCFG_DL_init();  // SysConfig 生成的初始化

    // 用硬件映射宏构建配置
    UART_Config cfg = {
        .uart         = UART_PRINT_INST,       // 外设基址（宏展开为 UART0）
        .irqNum       = UART_PRINT_INT_IRQN,   // 中断号
        .dmaTxChanId  = UART0_DMA_TX_CHAN_ID,  // DMA 通道号
        .dmaTxTrigger = PRINT_INST_DMA_TRIGGER, // DMA 触发源
    };
    uart_print = UART_Init(&cfg);

    // ... 开始使用
}
```

### 2. 发送数据

```c
// 阻塞发送（逐字节等待，适合调试输出）
UART_SendStr(uart_print, "Hello World\r\n");

// 阻塞格式化输出（printf 风格）
UART_Printf(uart_print, "Value: %d, Hex: 0x%04X\r\n", 42, 0xABCD);

// DMA 发送（非阻塞，速度更快）
UART_SendStrDMA(uart_print, "Fast send\r\n", 10);
while (!uart_print->txDMADone);  // 等待发送完成

// DMA 格式化输出
UART_PrintfDMA(uart_print, "Sensor: %d\r\n", value);
while (!uart_print->txDMADone);
```

### 3. 接收数据

```c
// 开始接收（重置接收缓冲区）
UART_StartReceive(uart_print);

// 在主循环中轮询接收完成标志
if (uart_print->rxDone) {
    // uart_print->rxBuf   — 接收到的字符串（已添加 '\0'）
    // uart_print->rxLen   — 接收长度
    UART_PrintfDMA(uart_print, "Echo: %s\r\n", uart_print->rxBuf);
    while (!uart_print->txDMADone);

    UART_StartReceive(uart_print);  // 继续接收下一次
}
```

## API 参考

### UART_Init

```c
UART_Handle* UART_Init(const UART_Config *config);
```

注册并初始化一个 UART 实例。分配句柄存储、设置硬件引用、初始化状态、使能 NVIC 中断。

| 参数 | 说明 |
|------|------|
| `config->uart` | UART 外设基址（`UART0` / `UART1` / `UART2` / `UART3`） |
| `config->irqNum` | NVIC 中断号 |
| `config->dmaTxChanId` | DMA TX 通道号 |
| `config->dmaTxTrigger` | DMA 发送触发源 |

返回值：成功返回句柄指针，失败返回 `NULL`（重复注册 / 无效外设 / 实例已满）。

### UART_SendStr

```c
int UART_SendStr(UART_Handle *h, const char *str);
```

阻塞方式发送字符串，逐字节等待直到全部发送完成。适合短字符串和调试输出。

### UART_Printf

```c
int UART_Printf(UART_Handle *h, char *fmt, ...);
```

阻塞方式格式化输出，内部调用 `vsprintf` + `UART_SendStr`。格式化缓冲区大小由 `UART_TX_BUF_SIZE`（256 字节）限制。

### UART_SendStrDMA

```c
void UART_SendStrDMA(UART_Handle *h, const char *str, uint16_t len);
```

DMA 方式发送字符串，**非阻塞**。函数返回时 DMA 传输可能尚未完成，需等待 `h->txDMADone == 1`。

### UART_PrintfDMA

```c
void UART_PrintfDMA(UART_Handle *h, char *fmt, ...);
```

DMA 方式格式化输出，**非阻塞**。同上，需等待 DMA 完成标志。

### UART_StartReceive

```c
void UART_StartReceive(UART_Handle *h);
```

开始接收数据。重置 `rxPos` 和 `rxDone`，之后由 ISR 自动填充缓冲区。

### UART_HandleIRQ

```c
void UART_HandleIRQ(UART_Handle *h);
```

通用中断处理函数。由各 `UARTx_IRQHandler` 调用，处理 DMA 发送完成和 RX 接收两种中断。用户一般不需要直接调用。

## 句柄结构体字段

| 字段 | 类型 | 说明 |
|------|------|------|
| `h->uart` | `UART_Regs*` | UART 外设基址指针 |
| `h->dma` | `DMA_Regs*` | DMA 控制器基址指针 |
| `h->dmaTxChanId` | `uint8_t` | DMA TX 通道号 |
| `h->txDMADone` | `volatile uint8_t` | 1 = DMA 空闲，0 = 传输中 |
| `h->rxDone` | `volatile uint8_t` | 1 = 接收完成（已收到结束符） |
| `h->rxBuf[]` | `volatile uint8_t[256]` | 接收缓冲区 |
| `h->rxLen` | `volatile uint16_t` | 本次接收长度（不含结束符） |
| `h->rxPos` | `volatile uint16_t` | 当前写入位置 |
| `h->rxOvf` | `volatile uint8_t` | 溢出字节（上次数据未取走时的新数据） |

## 添加多个 UART 实例

假设 SysConfig 中已配置第二个 UART（如 UART1，名称为 "GPS"）：

```c
UART_Handle *uart_gps;

void gps_init(void) {
    UART_Config cfg = {
        .uart         = GPS_INST,            // 来自 SysConfig
        .irqNum       = GPS_INST_INT_IRQN,
        .dmaTxChanId  = GPS_DMA_TX_CHAN_ID,
        .dmaTxTrigger = GPS_INST_DMA_TRIGGER,
    };
    uart_gps = UART_Init(&cfg);
}

// 两个 UART 独立使用，互不干扰
UART_Printf(uart_print, "Hello from PRINT\r\n");
UART_SendStr(uart_gps,   "Hello from GPS\r\n");
```

**ISR 无需额外代码** — `UART1_IRQHandler()` 已在 `uart.c` 中定义，会自动查找已注册的句柄并分发。

## SysConfig 重新生成后的适配

当 SysConfig 工具重新生成 `ti_msp_dl_config.h` 后，外设名称可能改变。只需修改 `uart.h` 中的**硬件映射宏**即可：

```c
// 修改前（旧名称）
#define UART_PRINT_INST             PRINT_INST
#define UART_PRINT_INT_IRQN         PRINT_INST_INT_IRQN
#define UART_PRINT_IRQHandler       PRINT_INST_IRQHandler

// 修改后（SysConfig 改了名称，如 PRINT → DEBUG_UART）
#define UART_PRINT_INST             DEBUG_UART_INST
#define UART_PRINT_INT_IRQN         DEBUG_UART_INT_IRQN
#define UART_PRINT_IRQHandler       DEBUG_UART_IRQHandler
```

应用代码中所有使用 `UART_PRINT_INST` 等宏的地方无需修改。

## ISR 分发机制

```
硬件中断
  → UARTx_IRQHandler()           // 强定义，覆盖 startup 中的弱别名
    → 查 g_uart_instances[x]     // 按 UART 索引查找已注册句柄
      → UART_HandleIRQ(handle)   // 通用中断处理
        → switch (IIDX)
            DMA_DONE_TX → handle->txDMADone = 1
            RX          → 读字节 → 检测结束符 → 置 handle->rxDone
```

每个 UART0~UART3 的 ISR 均已预定义在 `uart.c` 中。如果某个 UART 未注册（`g_uart_instances[x] == NULL`），ISR 不会执行任何操作。
