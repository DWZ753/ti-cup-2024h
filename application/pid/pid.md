# PID 控制器模块

## 概述

标准**位置式 PID**（Proportional-Integral-Derivative）控制器，提供积分限幅和输出限幅功能。适用于速度环、角度环、转向控制等各种闭环控制场景。

## 文件

| 文件 | 说明 |
|------|------|
| `pid.h` | 模块头文件，结构体定义与 API 声明 |
| `pid.c` | 模块实现 |

## 算法说明

### PID 公式

```
error = target - current_value
P_out = Kp × error
I_out = Ki × Σ(error)        （带积分限幅）
D_out = Kd × (error - 2×last_error + prev_error)   （二阶微分，抗噪）
output = P_out + I_out + D_out  （带输出限幅）
```

### 微分项设计

微分项使用**两个历史误差**的滑动平均来抑制噪声：
```c
d_out = Kd × (error - 2.0 × last_error + prev_error)
```
这比简单差分 `(error - last_error)` 对高频噪声更不敏感。

### 限幅保护

| 限幅类型 | 说明 |
|----------|------|
| 积分限幅 (`integral_limit`) | 防止积分项无限累加（积分饱和），导致系统超调失控 |
| 输出限幅 (`output_limit`) | 确保控制输出不超过执行器物理极限（如 PWM 满量程） |

## 类型定义

### `PID_Controller` — PID 控制器结构体

```c
typedef struct {
    float Kp;             // 比例系数
    float Ki;             // 积分系数
    float Kd;             // 微分系数
    float target;         // 目标值
    float error;          // 当前误差
    float last_error;     // 上一次误差
    float prev_error;     // 上上次误差（用于抗微分噪声）
    float integral;       // 积分累加项
    float integral_limit; // 积分限幅值
    float output_limit;   // 输出限幅值
    float output;         // 最终输出结果
} PID_Controller;
```

## API

### `void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float integral_limit, float output_limit)`

初始化 PID 控制器。

| 参数 | 说明 |
|------|------|
| `pid` | 控制器结构体指针 |
| `kp` | 比例系数 |
| `ki` | 积分系数 |
| `kd` | 微分系数 |
| `integral_limit` | 积分限幅（建议取输出限幅的 50%~80%） |
| `output_limit` | 输出限幅（对应执行器最大值，如 PWM 满量程） |

### `float PID_Compute(PID_Controller *pid, float current_value)`

执行一次 PID 计算。

| 参数 | 说明 |
|------|------|
| `pid` | 控制器结构体指针 |
| `current_value` | 当前反馈值（传感器读数） |

| 返回值 | 说明 |
|--------|------|
| `float` | 控制输出量（已限幅） |

**调用频率：** 应在固定的控制周期内调用，如 20ms（PIT Control Tick）。

### `void PID_SetTarget(PID_Controller *pid, float target)`

设置新的目标值。

| 参数 | 说明 |
|------|------|
| `target` | 目标值 |

### `void PID_Reset(PID_Controller *pid)`

清零积分项和误差历史。**模式切换时必须调用**，避免旧状态的积分值影响新模式。

## 使用示例

### 电机速度环 PID

```c
#include “pid.h”
#include “motor.h”

// 定义控制器
PID_Controller speed_pid;

// 初始化
PID_Init(&speed_pid,
    0.5,      // Kp: 从 0.5 开始调
    0.1,      // Ki: 从 0.1 开始调
    0.01,     // Kd: 从 0.01 开始调
    500.0,    // 积分限幅
    1000.0);  // 输出限幅（对应 PWM 满幅）
PID_SetTarget(&speed_pid, 500.0);  // 目标速度 500mm/s

// 在 PIT Control Tick 回调中计算
void Motor_PID_TickHandler(void) {
    float current_speed = Motor_GetEncoder1Speed();
    float output = PID_Compute(&speed_pid, current_speed);
    Motor_SetSpeed(output);
}
```

### 转向循迹 PID

```c
PID_Controller steering_pid;
PID_Init(&steering_pid, 2.0, 0.0, 0.5, 100.0, 100.0);
PID_SetTarget(&steering_pid, 0.0f);  // 目标：黑线居中

void Tracking_TickHandler(void) {
    uint8_t mask = Grayscale_ReadAll();
    float position = Tracking_CalcPosition(mask);
    float steering = PID_Compute(&steering_pid, position);
    Servo_SetValue((int32_t)steering);
}
```

## PID 调参指南

### 口诀

> 参数整定找最佳，从小到大顺序查。
> 先是比例后积分，最后再把微分加。

### 实操步骤

1. **纯 P 调试**：Ki=0, Kd=0，从 0.1 开始逐步增大 Kp
   - 电机出现高频振动 → 回退到振动前值的 70%
2. **加入 I**：从 0.01 开始逐步增大 Ki
   - 加快消除稳态误差，注意不要过冲
3. **加入 D**：速度环通常不需要 D，转向环可加少量（如 0.001~0.01）
   - 抑制振荡，相当于”超前刹车”

### 常见问题

| 现象 | 调整方向 |
|------|----------|
| 响应太慢 | 增大 Kp |
| 振荡频繁 | 减小 Kp / 增大 Kd |
| 有稳态误差 | 增大 Ki |
| 过冲严重 | 减小 Ki / 增大 Kd |
| 积分饱和 | 减小 integral_limit |

## 依赖

- `ti_msp_dl_config.h`