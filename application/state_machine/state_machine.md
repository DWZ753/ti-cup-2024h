# State Machine 状态机模块

## 概述

简单的任务状态机模块，通过按键触发切换运行状态（任务模式）。支持 IDLE + TASK1~TASK4 共 5 个状态，按键 1~4 分别对应切换到 TASK1~TASK4。

## 文件

| 文件 | 说明 |
|------|------|
| `state_machine.h` | 模块头文件，状态枚举与 API 声明 |
| `state_machine.c` | 模块实现 |

## 状态定义

```c
typedef enum {
    STATE_IDLE = 0,   // 空闲/默认状态
    STATE_TASK1,      // 任务 1（对应按键 1）
    STATE_TASK2,      // 任务 2（对应按键 2）
    STATE_TASK3,      // 任务 3（对应按键 3）
    STATE_TASK4       // 任务 4（对应按键 4）
} QuestionState_t;
```

## 核心机制

### 按键 → 状态映射

模块内部注册按键回调，按键索引到状态的映射为：

| 按键 | key_index | 切换到的状态 |
|------|-----------|-------------|
| KEY1 | 0 | `STATE_TASK1` |
| KEY2 | 1 | `STATE_TASK2` |
| KEY3 | 2 | `STATE_TASK3` |
| KEY4 | 3 | `STATE_TASK4` |

每次按键触发时，LED 翻转作为视觉反馈。

## API

### `void StateMachine_Init(void)`

初始化状态机，注册按键回调。

> 此函数在 `main.c` 中调用（在 `Board_Init()` 之后），而非在 `Board_Init()` 内部。

### `QuestionState_t StateMachine_GetState(void)`

获取当前状态。

| 返回值 | 说明 |
|--------|------|
| `STATE_IDLE` (0) | 空闲状态 |
| `STATE_TASK1` ~ `STATE_TASK4` (1~4) | 对应任务状态 |

## 依赖

- [Key 按键模块](../../modules/key/key.md)（注册按键回调）

## 使用示例

```c
#include "state_machine.h"

// 初始化（在 main.c 中调用）
StateMachine_Init();

// 在主循环中根据状态执行不同任务
void main_loop(void) {
    QuestionState_t state = StateMachine_GetState();

    switch (state) {
        case STATE_IDLE:
            // 空闲，等待按键
            break;
        case STATE_TASK1:
            // 执行任务 1（如循迹模式）
            break;
        case STATE_TASK2:
            // 执行任务 2（如避障模式）
            break;
        case STATE_TASK3:
            // 执行任务 3
            break;
        case STATE_TASK4:
            // 执行任务 4
            break;
    }
}
```

## 扩展指南

如需添加更多状态或修改按键映射，编辑 `state_machine.c`：

```c
// 1. 在 QuestionState_t 枚举中添加新状态
// 2. 修改 Key_Handler 中的按键→状态映射逻辑

static void Key_Handler(uint8_t key_index) {
    s_current_state = (QuestionState_t)(key_index + 1);
    // 可以在这里添加更复杂的映射逻辑
    DL_GPIO_togglePins(GPIO_LEDs_PORT, GPIO_LEDs_GPIO_LED_PIN);
}
```
