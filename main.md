# main.c — 主程序

## 概述

`main.c` 是整个项目的入口。

当前版本主要做了两件事：**IMU 姿态解算** 和 **UART 遥测输出**，用于验证传感器和通信链路是否正常。

## 初始化顺序

初始化分为三步，**前两步基本固定，第三步按需增减**：

```c
int main(void) {
    SYSCFG_DL_init();    // 1. 固定：SysConfig 生成的时钟和外设初始化
    Board_Init();        // 2. 基本固定：所有模块初始化（如需增删模块，改 board.c）
    StateMachine_Init(); // 3. 可选：状态机初始化。不需要状态机时删掉即可

    // ... 进入主循环（这里是你可以自由发挥的地方）
}
```

## 主循环调度（当前版本）

当前使用基于 `Board_GetTickMs()` 的非阻塞轮询：

| 任务 | 周期 | 说明 |
|------|------|------|
| `IMU_Update()` | 2ms | 姿态解算（如果你不需要 IMU，可以删掉或改周期） |
| UART 遥测输出 | 10ms | 向上位机输出调试数据（格式自定义，按需改） |

```c
uint32_t last_imu    = 0;
uint32_t last_output = 0;

while (1) {
    uint32_t now = Board_GetTickMs();

    if (now - last_imu >= 2) {
        last_imu = now;
        IMU_Update();
    }

    if (now - last_output >= 10) {
        last_output = now;
        // ... 串口输出（内容根据需求自定义）
    }
}
```

## 当前 UART 遥测格式

每 10ms 输出一行 CSV：

```
q0,q1,q2,q3,roll,pitch,yaw,ax,ay,az
```

| 列 | 说明 | 单位 |
|----|------|------|
| q0~q3 | 四元数 | 无量纲 |
| roll / pitch / yaw | 欧拉角 | 度 (°) |
| ax / ay / az | 加速度 | m/s² |

## 常用模式参考

以下是一些常见的主循环组织方式。

### 模式一：添加新任务

在 `while(1)` 中按同样模式添加：

```c
uint32_t last_my_task = 0;
while (1) {
    uint32_t now = Board_GetTickMs();

    // ... 其他任务 ...

    if (now - last_my_task >= N) {   // N 是你需要的周期（ms）
        last_my_task = now;
        MyTask_Execute();
    }
}
```

### 模式二：配合状态机切换行为

通过 [State Machine](application/state_machine/state_machine.md) 在不同赛题模式间切换：

```c
while (1) {
    switch (StateMachine_GetState()) {
        case STATE_IDLE:
            // 空闲等待
            break;
        case STATE_TASK1:
            // 循迹模式：读灰度 → 算位置 → 控舵机+电机
            break;
        case STATE_TASK2:
            // 其他模式...
            break;
    }
}
```

### 模式三：按固定顺序执行（无定时）

对于简单任务，甚至不需要定时调度：

```c
while (1) {
    sensor_read();
    control_calc();
    motor_output();
    delay_ms(20);
}
```

## 注意事项

- 主循环使用**忙等轮询**（busy-wait polling），无 RTOS。各模块需要严格周期性的任务通过 PIT 中断回调完成。
