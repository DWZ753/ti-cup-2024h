# Board 板级初始化与系统管理

## 概述

`board.c` / `board.h` 是系统的**初始化入口**和**全局资源汇总点**。它负责按依赖顺序初始化各硬件模块，并提供系统滴答时钟等公共服务。

实际开发中，当你新增或移除模块时，`board.c` 和 `board.h` 是**需要同步修改的两个文件**。

## 文件

| 文件 | 说明 |
|------|------|
| `board.h` | 汇总各模块头文件 + 声明公共 API。新增模块时在这里加 `#include` |
| `board.c` | 初始化实现 + 全局资源。新增模块时在这里加 `_Init()` 调用 |

## 初始化流程（当前版本）

`Board_Init()` 在 `main.c` 中 `SYSCFG_DL_init()` 之后调用。当前按以下顺序初始化：

```
PIT_Fast_Tick_Init()              // 1ms 定时器
PIT_Fast_Tick_RegisterCallback()  // 注册 IMU 滴答计数器
PIT_Control_Tick_Init()           // 20ms 定时器

Buzzer_Init()                     // 蜂鸣器
TB6612_Init()                     // 电机驱动
Motor_Init()                      // 电机模块
Servo_Init()                      // 舵机

Key_Init()                        // 按键
Grayscale_Init()                  // 灰度传感器

UART_Init()                       // 串口
IMU_Init()                        // IMU 姿态传感器
OLED_Init()                       // OLED 显示屏
```

> 大原则：**定时器 → 执行器 → 输入 → 通信 → 传感器 → 显示**。新增模块时按此原则插入合适的位子。用不到某个模块时直接删掉即可。

## API

### `void Board_Init(void)`

板级初始化。调用前需确保 `SYSCFG_DL_init()` 已执行。

**增删模块时修改此函数**：新增模块加 `xxx_Init()`，不用的模块删掉。

### `uint32_t Board_GetTickMs(void)`

获取系统 1ms 滴答计数，用于 `main.c` 中的非阻塞定时。

### `UART_Handle* Board_GetUART(void)`

获取当前注册的 UART 句柄。如果你添加了更多 UART 实例，可以按同样模式提供对应的 getter。

## 系统滴答时钟

1ms 计数器由 PIT Fast Tick 中断累加：

```c
static volatile uint32_t imu_ticks;

static void imu_tick_cb(void) {
    imu_ticks++;
}
```

## 新增模块时的修改清单

假设你写了一个新模块 `modules/xxx/`，需要：

1. **`board.h`**：添加 `#include "xxx.h"`
2. **`board.c`**：在 `Board_Init()` 中适当位置调用 `Xxx_Init()`
3. 如果新模块需要主循环中轮询，去 `main.c` 加逻辑

如果新模块依赖某个已有模块（比如新传感器用 SPI），确认依赖项在它之前初始化。

## 注意事项

- 初始化顺序有依赖关系：**定时器必须先于使用其回调的模块**，**通信层（SPI/I2C）必须先于使用该总线的传感器**
- `imu_ticks` 是 `uint32_t`，连续运行约 49.7 天会回绕，但 `now - last >= N` 的比较对无符号数回绕是安全的
- `board.h` 是几乎所有 `.c` 文件都会 include 的，新增的内容尽量精简，避免编译变慢
