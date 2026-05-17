# PIT_CUSTOM_TICK 模块使用说明文档

## 📋 目录
- [概述](#概述)
- [功能特性](#功能特性)
- [API 接口](#api-接口)
- [快速开始](#快速开始)
- [使用示例](#使用示例)
- [注意事项](#注意事项)
- [常见问题](#常见问题)

---

## 概述

**PIT_CUSTOM_TICK** 是一个基于 TI MSPM0 系列微控制器的可编程间隔定时器（PIT）自定义滴答模块。该模块提供周期性定时器中断功能，允许用户注册多个回调函数在定时器中断中周期性执行，适用于需要精确时间控制的嵌入式应用场景。

### 核心特点
- 基于硬件 PIT 定时器，精度高
- 支持多任务回调注册（最多 8 个）
- 简单的 API 接口，易于使用
- 适用于系统心跳、周期性任务调度等场景

---

## 功能特性

### 主要功能
1. **周期性中断**：基于 PIT 定时器产生周期性中断
2. **多任务支持**：最多可同时注册 8 个回调任务
3. **自动调度**：中断发生时自动依次执行所有注册的回调函数
4. **线程安全**：提供基本的空指针检查和任务数量限制

### 技术规格
- **最大任务数**：8 个回调函数
- **中断源**：[PIT_FOR_CUSTOM_INST] 定时器 LOAD 中断
- **中断向量**：`TIMG12_IRQHandler`
- **回调类型**：`void (*)(void)` - 无参数无返回值

---

## API 接口

### 1. PIT_Custom_Tick_Init()

**功能**：初始化 PIT 自定义滴答定时器中断

**原型**：
```c
void PIT_Custom_Tick_Init(void);
```

**参数**：无

**返回值**：无

**说明**：
- 使能 [PIT_FOR_CUSTOM_INST_INT_IRQN]中断
- 必须在注册任何回调函数之前调用
- 通常在中断向量表配置完成后调用

**示例**：
```c
int main(void)
{
    SYSCFG_DL_init();  // 系统配置初始化
    
    PIT_Custom_Tick_Init();  // 初始化 PIT 滴答定时器
    
    // ... 其他初始化代码
}
```

---

### 2. PIT_Custom_Tick_RegisterCallback()

**功能**：注册 PIT 自定义滴答定时器回调函数

**原型**：
```c
bool PIT_Custom_Tick_RegisterCallback(PIT_Custom_Callback_t callback);
```

**参数**：
- `callback`：要注册的回调函数指针，类型为 `PIT_Custom_Callback_t`（即 `void (*)(void)`）

**返回值**：
- `true`：注册成功
- `false`：注册失败（任务数量已达上限或回调函数指针为空）

**说明**：
- 最多可注册 8 个回调函数
- 回调函数将在每次 PIT 定时器 LOAD 中断时被调用
- 重复注册会按顺序添加到任务列表
- 回调函数应尽量简短，避免影响系统实时性

**示例**：
```c
void myTaskHandler(void)
{
    // 周期性执行的任务代码
}

// 注册回调
if (PIT_Custom_Tick_RegisterCallback(myTaskHandler))
{
    // 注册成功
}
else
{
    // 注册失败，处理错误
}
```

---

### 3. PIT_Custom_Callback_t（类型定义）

**定义**：
```c
typedef void (*PIT_Custom_Callback_t)(void);
```

**说明**：
- 回调函数类型定义
- 所有注册的回调函数必须符合此签名
- 无参数，无返回值

---

## 快速开始

### 步骤 1：包含头文件

```c
#include "pit_custom_tick.h"
```

### 步骤 2：定义回调函数

创建一个符合 `PIT_Custom_Callback_t` 签名的函数：

```c
static volatile uint32_t tickCount = 0;

void systemTickHandler(void)
{
    tickCount++;
    
    // 在此添加周期性执行的代码
    // 例如：更新状态、处理传感器数据等
}
```

### 步骤 3：初始化 PIT 定时器

在系统启动时调用初始化函数：

```c
int main(void)
{
    SYSCFG_DL_init();  // 系统配置初始化（包含 PIT 硬件配置）
    
    PIT_Custom_Tick_Init();  // 使能 PIT 中断
    
    // ... 其他初始化
}
```

### 步骤 4：注册回调函数

```c
int main(void)
{
    SYSCFG_DL_init();
    PIT_Custom_Tick_Init();
    
    // 注册回调函数
    if (!PIT_Custom_Tick_RegisterCallback(systemTickHandler))
    {
        // 处理注册失败
        while(1);
    }
    
    // 主循环
    while(1)
    {
        // 主程序逻辑
    }
}
```

---

## 使用示例

### 示例 1：基础用法 - 单任务

```c
#include "pit_custom_tick.h"
#include <stdint.h>

static volatile uint32_t systemTicks = 0;

// 系统滴答回调函数
void SystemTick_Handler(void)
{
    systemTicks++;
}

int main(void)
{
    SYSCFG_DL_init();
    PIT_Custom_Tick_Init();
    
    // 注册系统滴答回调
    PIT_Custom_Tick_RegisterCallback(SystemTick_Handler);
    
    while(1)
    {
        // 使用 systemTicks 进行时间相关操作
        if (systemTicks >= 1000)
        {
            // 每秒执行一次的任务
            systemTicks = 0;
        }
    }
}
```

### 示例 2：多任务调度

```c
#include "pit_custom_tick.h"
#include <stdint.h>
#include <stdbool.h>

static volatile uint32_t ledToggleCounter = 0;
static volatile uint32_t sensorReadCounter = 0;

// LED 闪烁任务
void LED_Task(void)
{
    ledToggleCounter++;
    if (ledToggleCounter >= 500)  // 每 500ms 切换一次
    {
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN);
        ledToggleCounter = 0;
    }
}

// 传感器读取任务
void Sensor_Task(void)
{
    sensorReadCounter++;
    if (sensorReadCounter >= 100)  // 每 100ms 读取一次
    {
        // 读取传感器数据
        sensorReadCounter = 0;
    }
}

int main(void)
{
    SYSCFG_DL_init();
    PIT_Custom_Tick_Init();
    
    // 注册多个任务
    PIT_Custom_Tick_RegisterCallback(LED_Task);
    PIT_Custom_Tick_RegisterCallback(Sensor_Task);
    
    while(1)
    {
        // 主循环处理其他任务
    }
}
```

### 示例 3：蜂鸣器控制（实际项目应用）

```c
#include "pit_custom_tick.h"
#include <stdint.h>
#include <stdbool.h>

static volatile uint32_t s_beep_time = 0;
static bool s_is_active = false;

// 蜂鸣器 tick 处理回调
static void Buzzer_TickHandler(void)
{
    if (!s_is_active) return;

    if (s_beep_time > 0)
    {
        DL_GPIO_clearPins(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
        s_beep_time--;
    }
    else
    {
        // 时间到，停止蜂鸣器
        s_is_active = false;
        DL_GPIO_setPins(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
    }
}

// 蜂鸣器初始化
void Buzzer_Init(void)
{
    PIT_Custom_Tick_RegisterCallback(Buzzer_TickHandler);
}

// 启动蜂鸣器
void Buzzer_Start(uint32_t time_ms)
{
    s_beep_time = time_ms;
    s_is_active = true;
}

// 停止蜂鸣器
void Buzzer_Stop(void)
{
    s_is_active = false;
    s_beep_time = 0;
    DL_GPIO_setPins(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
}
```

### 示例 4：完整系统初始化流程

```c
#include "pit_custom_tick.h"
#include "buzzer.h"
#include "motor.h"

int main(void)
{
    // 1. 系统配置初始化（包含 PIT 硬件配置）
    SYSCFG_DL_init();

    // 2. 初始化 PIT 自定义滴答定时器
    PIT_Custom_Tick_Init();
    
    // 3. 初始化各个模块（内部会注册各自的回调）
    Buzzer_Init();      // 注册蜂鸣器回调
    TB6612_Init();      // 注册电机驱动回调
    Motor_Init();       // 注册电机控制回调

    // 4. 主循环
    while (1)
    {
        // 启动蜂鸣器 1000ms
        Buzzer_Start(1000);
        
        // 电机正转，速度 50%
        Motor_Forward(50);
        
        // 延时 2 秒
        delay_ms(2000);
    }
}
```

---

## 注意事项

### ⚠️ 重要提醒

#### 1. 回调函数设计原则
- ✅ **保持简短**：回调函数在中断上下文中执行，应尽可能简短
- ✅ **避免阻塞**：不要在回调中使用延时、等待等阻塞操作
- ✅ **避免重入**：避免调用可能引起重入的函数（如 printf、malloc 等）
- ❌ **禁止休眠**：不能在中断中调用可能导致任务切换的操作

#### 2. 线程安全问题
- 使用 `volatile` 关键字修饰在中断和主程序之间共享的变量
- 考虑使用原子操作或临界区保护共享资源
- 避免在回调中修改复杂数据结构

```c
// 正确做法
static volatile uint32_t sharedCounter = 0;

void TickHandler(void)
{
    sharedCounter++;  // 简单原子操作
}

// 在主程序中读取
uint32_t current = sharedCounter;
```

#### 3. 任务数量限制
- 当前实现最多支持 **8 个** 回调函数
- 超过限制时 `RegisterCallback` 返回 `false`
- 如需更多任务，需修改 `MAX_PIT_CUSTOM_TASKS` 宏定义

#### 4. 定时器周期
- 定时器的触发周期由底层 PIT 配置决定
- 具体周期值请参考 [ti_msp_dl_config.h] 中的 PIT 配置
- 常见配置为 1ms（1kHz）周期

#### 5. 初始化顺序
```
正确顺序：
1. SYSCFG_DL_init()     // 系统配置（包含 PIT 硬件初始化）
2. PIT_Custom_Tick_Init()  // 使能 PIT 中断
3. 注册各模块回调函数
4. 启动定时器（如果需要）
```

#### 6. 中断服务程序
- ISR 名称固定为 `TIMG12_IRQHandler` 或 [PIT_FOR_CUSTOM_INST_IRQHandler]
- 不要手动调用 ISR
- ISR 会自动清除中断标志并执行所有注册的回调

---

## 常见问题

### Q1: 回调函数没有被执行？

**可能原因**：
1. 未调用 [PIT_Custom_Tick_Init()]
2. PIT 定时器未启动（检查 `SYSCFG_DL_init()` 是否包含定时器启动）
3. 中断未正确配置

**解决方法**：
```c
// 确保正确的初始化顺序
SYSCFG_DL_init();           // 包含 PIT 硬件配置和启动
PIT_Custom_Tick_Init();     // 使能 NVIC 中断
PIT_Custom_Tick_RegisterCallback(myCallback);
```

### Q2: 如何知道定时器的周期是多少？

**查看方法**：
1. 打开 [ti_msp_dl_config.h] 文件
2. 查找 [PIT_FOR_CUSTOM_INST] 相关配置
3. 查看定时器时钟源和重载值

**典型配置**：
- 时钟源：32MHz
- 重载值：32000
- 周期：1ms（1kHz）

### Q3: 如何在回调中安全地更新全局变量？

**推荐做法**：
```c
// 使用 volatile 修饰
static volatile uint32_t counter = 0;

void TickHandler(void)
{
    counter++;  // 简单递增是安全的
}

// 在主程序中读取
uint32_t getCounter(void)
{
    __disable_irq();  // 临时禁用中断
    uint32_t value = counter;
    __enable_irq();   // 恢复中断
    return value;
}
```

### Q4: 可以动态取消注册回调吗？

**当前实现**：不支持动态取消注册

**替代方案**：
```c
// 在回调内部使用标志位控制
static bool taskEnabled = true;

void MyTask(void)
{
    if (!taskEnabled) return;
    
    // 执行任务逻辑
}

// 禁用任务
taskEnabled = false;
```

### Q5: 多个回调的执行顺序是什么？

**执行顺序**：按照注册顺序依次执行

```c
PIT_Custom_Tick_RegisterCallback(TaskA);  // 先执行
PIT_Custom_Tick_RegisterCallback(TaskB);  // 后执行
PIT_Custom_Tick_RegisterCallback(TaskC);  // 最后执行
```

**注意**：如果某个回调执行时间过长，会影响后续回调的执行时机。

### Q6: 如何处理回调中的错误？

**建议做法**：
```c
void SafeTaskHandler(void)
{
    // 使用 try-catch 风格的错误处理
    if (someCondition)
    {
        // 记录错误，但不要抛出异常
        errorFlag = true;
        return;
    }
    
    // 正常处理逻辑
}
```

**注意**：中断中不能使用 C++ 异常机制。

---

## 附录

### 相关文件
- `modules/pit_custom_tick.h` - 头文件，包含 API 声明
- `modules/pit_custom_tick.c` - 实现文件
- [ti_msp_dl_config.h] - 系统配置文件（包含 PIT 硬件配置）

### 依赖项
- TI DriverLib 库
- TI MSPM0 SDK
- CMSIS Core（用于 NVIC 操作）

### 版本历史
- **v1.0**：初始版本，支持最多 8 个回调任务

### 技术支持
如有问题，请参考：
- TI MSPM0 系列技术参考手册
- TI DriverLib 用户指南
- 项目文档和示例代码

---

**文档生成日期**：2026-05-13  
**适用平台**：TI MSPM0 系列微控制器  
**编译器**：TI Clang ARM Compiler