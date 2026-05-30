# IMU 惯性测量单元模块

## 概述

IMU 姿态解算模块，封装 BMI088 六轴传感器（加速度计 + 陀螺仪）的驱动和 Mahony 互补滤波算法，对外输出**四元数**、**欧拉角**（Roll/Pitch/Yaw）和**原始传感器数据**。

## 文件

| 文件 | 说明 |
|------|------|
| `imu.h` | IMU 模块头文件 |
| `imu.c` | IMU 模块实现（组装 BMI088 + Mahony） |
| `bmi088/bmi088.h` | BMI088 传感器驱动头文件 |
| `bmi088/bmi088.c` | BMI088 传感器驱动实现 |
| `bmi088/mahony.h` | Mahony 互补滤波器头文件 |
| `bmi088/mahony.c` | Mahony 互补滤波器实现 |
| `bmi088/deewaz_math.h` | 数学工具（三维向量、弧度转换、快速平方根倒数） |

## 架构

```
IMU_Init()          → SPI_Init() + BMI088_Init() + Mahony_Init()
IMU_Update()        → BMI088_ReadAccel() + BMI088_ReadGyro() + Mahony_Input/Update/Output
IMU_GetEuler()      → 返回 mahony.roll / pitch / yaw
IMU_GetQuaternion() → 返回 mahony.q0~q3
IMU_GetAccel()      → 返回原始加速度 [x, y, z]
IMU_GetGyro()       → 返回原始角速度 [x, y, z]
```

## API

### `void IMU_Init(void)`

初始化 IMU 子系统。内部依次完成：
1. 注册 SPI 实例（SPI0，用于 BMI088 通信）
2. BMI088 软复位（加速度计 + 陀螺仪分别复位）
3. Mahony 滤波器配置（Kp=18.0, Ki=0.002, dt=0.002s）

> 此函数由 `Board_Init()` 自动调用。

### `void IMU_Update(void)`

执行一次 IMU 采样 + 姿态解算。

1. 读取加速度计（m/s²）
2. 读取陀螺仪（rad/s）
3. 送入 Mahony 互补滤波器
4. 更新四元数 → 旋转矩阵 → 欧拉角

> 建议每 **2ms** 调用一次（与滤波器 `dt=0.002s` 一致）。`main.c` 中通过 `Board_GetTickMs()` 每 2ms 调用一次。

### `void IMU_GetEuler(float *roll, float *pitch, float *yaw)`

获取欧拉角（单位：度）。

| 参数 | 说明 |
|------|------|
| `roll` | 输出横滚角 (°) |
| `pitch` | 输出俯仰角 (°) |
| `yaw` | 输出偏航角 (°) |

### `void IMU_GetQuaternion(float *q0, float *q1, float *q2, float *q3)`

获取四元数各分量。

### `void IMU_GetAccel(float accel[3])`

获取最新加速度计数据。输出 `[x, y, z]`，单位 m/s²。

### `void IMU_GetGyro(float gyro[3])`

获取最新陀螺仪数据。输出 `[x, y, z]`，单位 rad/s。

## BMI088 传感器驱动

### 硬件连接

| 信号 | 引脚 | 说明 |
|------|------|------|
| SPI_SCK | PB10 | SPI 时钟 |
| SPI_MOSI | PB11 | SPI 主机发从机收 |
| SPI_MISO | PB12 | SPI 从机发主机收 |
| ACCEL_CS | PB13 | 加速度计片选（低有效） |
| GYRO_CS | PB15 | 陀螺仪片选（低有效） |

### 传感器参数

| 参数 | 值 | 说明 |
|------|-----|------|
| 加速度计量程 | ±6g | 由 `BMI088_ACCEL_RANGE_6G` 定义 |
| 加速度灵敏度 | 0.001795 m/s²/LSB | = (6×9.8)/32768 |
| 陀螺仪量程 | ±2000°/s | 由 `BMI088_GYRO_RANGE_2000` 定义 |
| 陀螺灵敏度 | 0.001065264436 rad/s/LSB | = (2000×π/180)/32768 |

### BMI088 API

| 函数 | 说明 |
|------|------|
| `BMI088_Init(SPI_Handle *spi)` | 初始化传感器（软复位），返回 0 成功 |
| `BMI088_ReadAccel(float accel[3])` | 读取加速度计，输出 m/s² |
| `BMI088_ReadGyro(float gyro[3])` | 读取陀螺仪（含 Chip ID 校验），输出 rad/s |
| `BMI088_ReadAccelID()` | 读取加速度计 Chip ID（应为 0x1E） |
| `BMI088_ReadGyroID()` | 读取陀螺仪 Chip ID（应为 0x0F） |
| `BMI088_ReadRawBytes(...)` | 裸读寄存器（调试用） |

### 陀螺仪 Chip ID 校验

`BMI088_ReadGyro()` 读取时先读 Chip ID 寄存器（0x00），校验是否为 `0x0F`。校验通过才读取角速度数据，否则 `gyro` 数组保持不变。

## Mahony 互补滤波器

Mahony 滤波器是一种基于四元数的姿态解算算法，通过加速度计修正陀螺仪积分漂移。

### 滤波器参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| Kp | 18.0 | 比例增益（越大 → 收敛越快但噪声敏感） |
| Ki | 0.002 | 积分增益（补偿陀螺仪常值零偏） |
| dt | 0.002s | 采样周期（对应 500Hz，与 2ms 调用频率一致） |

### Mahony API

| 函数 | 说明 |
|------|------|
| `Mahony_Init(mf, Kp, Ki, dt)` | 初始化滤波器，配置增益与采样周期 |
| `Mahony_Input(mf, gyro, acc)` | 输入最新陀螺仪和加速度计数据 |
| `Mahony_Update(mf)` | 执行一次滤波更新（核心算法） |
| `Mahony_Output(mf)` | 从旋转矩阵解算欧拉角 |

### 滤波算法流程

```
1. 加速度归一化（invSqrt 快速平方根倒数）
2. 计算加速度与重力方向的叉积误差
3. 积分误差累加（Ki × error × dt）
4. PI 修正陀螺仪原始值（gyro += Kp×error + errorInt）
5. 一阶龙格库塔法更新四元数
6. 四元数归一化
7. 更新旋转矩阵 → 解算欧拉角
```

## 数学工具 (deewaz_math.h)

| 定义 | 值/说明 |
|------|---------|
| `Axis3f` | 三维浮点向量结构体 `{x, y, z}` |
| `DEG2RAD` | 角度转弧度：`π/180` |
| `RAD2DEG` | 弧度转角度：`180/π` |
| `invSqrt(x)` | 快速平方根倒数（向量归一化用），实际调用 `1.0/sqrtf(x)` |

## 依赖

- [SPI 模块](../../bsp/spi/spi.md)
- `ti_msp_dl_config.h`

## 使用示例

```c
#include "imu.h"

// 初始化（由 Board_Init 自动调用）
IMU_Init();

// 在主循环中每 2ms 更新一次
void main_loop(void) {
    uint32_t last_imu = Board_GetTickMs();
    while (1) {
        if (Board_GetTickMs() - last_imu >= 2) {
            last_imu = Board_GetTickMs();
            IMU_Update();
        }
    }
}

// 获取姿态
float roll, pitch, yaw;
IMU_GetEuler(&roll, &pitch, &yaw);
printf("Roll: %.2f, Pitch: %.2f, Yaw: %.2f\n", roll, pitch, yaw);

// 获取四元数
float q0, q1, q2, q3;
IMU_GetQuaternion(&q0, &q1, &q2, &q3);

// 获取原始传感器数据
float accel[3], gyro[3];
IMU_GetAccel(accel);
IMU_GetGyro(gyro);
```

## 注意事项

- **Cortex-M0+ 无硬件 FPU**：所有浮点运算为软件模拟，IMU_Update 包含大量浮点运算，耗时较长。确保调用频率不超过 500Hz
- **Yaw 漂移**：Mahony 滤波器仅用加速度计修正 Roll/Pitch，Yaw 角会随时间漂移（无磁力计校正）
- **采样周期一致性**：`IMU_Update()` 的调用周期应尽量稳定在 2ms，否则需调整 `Mahony_Init()` 的 `dt` 参数
