# Tracking 循迹算法模块

## 概述

基于 8 路灰度传感器数据的黑线位置解算算法。使用**加权插值法**将传感器的离散电平值转换为连续的归一化位置值，供转向控制使用。

## 文件

| 文件 | 说明 |
|------|------|
| `tracking.h` | 模块头文件，宏定义与 API 声明 |
| `tracking.c` | 模块实现 |

## 算法原理

### 加权平均法

对检测到黑线（低电平）的传感器，以其索引为权重计算加权平均位置：

```
1. 取反掩码：bits = ~mask   （使 1 = 黑线/压线）
2. 加权求和：sum_weight = Σ(i × is_black[i])
3. 计数：    count = Σ(is_black[i])
4. 平均位置：center = sum_weight / count
5. 归一化：  position = (center - SENSOR_CENTER) / SENSOR_CENTER
```

其中 `SENSOR_CENTER = (8 - 1) / 2 = 3.5`。

### 返回值含义

| 返回值 | 含义 |
|--------|------|
| **-1.0** | 黑线在最左侧 |
| **0.0** | 黑线在正中间 |
| **+1.0** | 黑线在最右侧 |
| **99.0** | **丢线**：所有传感器均未检测到黑线 |

## 宏定义

```c
#define SENSOR_COUNT    8                 // 传感器数量
#define SENSOR_CENTER   ((SENSOR_COUNT - 1) / 2.0f)  // 中心索引 = 3.5
```

## API

### `float Tracking_CalcPosition(uint8_t mask)`

根据灰度传感器原始读数计算黑线的归一化位置。

| 参数 | 说明 |
|------|------|
| `mask` | 8 位掩码，由 `Grayscale_ReadAll()` 获取 |
| | bit[i]：1 = 白色/未压线，0 = 黑色/压线 |

| 返回值 | 说明 |
|--------|------|
| `[-1.0, +1.0]` | 归一化位置，负值偏左、正值偏右 |
| `99.0` | 所有传感器均未检测到黑线（丢线） |

## 依赖

- [Grayscale 灰度传感器模块](../../modules/grayscale/grayscale.md)（提供 `mask` 输入）

## 使用示例

```c
#include "grayscale.h"
#include "tracking.h"

// 读取传感器 → 计算位置 → 控制舵机
uint8_t mask = Grayscale_ReadAll();
float position = Tracking_CalcPosition(mask);

if (position == 99.0f) {
    // 丢线处理：保持上一次方向或停车
} else {
    // 位置映射到舵机角度
    // position = -1.0（偏左）→ 舵机左转修正
    // position = +1.0（偏右）→ 舵机右转修正
    int32_t servo_value = (int32_t)(position * 100.0f);
    Servo_SetValue(servo_value);
}
```

### 配合 PID 转向控制

```c
// 将位置偏差作为 PID 输入
PID_Controller steering_pid;
PID_Init(&steering_pid, 2.0, 0.0, 0.5, 100.0, 100.0);
PID_SetTarget(&steering_pid, 0.0f);  // 目标：黑线居中

float position = Tracking_CalcPosition(Grayscale_ReadAll());
float steering_output = PID_Compute(&steering_pid, position);
Servo_SetValue((int32_t)steering_output);
```

## 注意事项

- `mask` 中 bit 顺序与传感器物理排列顺序必须一致（bit[0] = 最左侧传感器）
- 丢线时返回 99.0，调用者需自行处理（保持上一帧位置 / 停车 / 倒车找回）
- 加权平均法对噪声较敏感，必要时在位置输出端加低通滤波
- 如果只有 1 个传感器检测到黑线，位置即为该传感器的归一化索引
