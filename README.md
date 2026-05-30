# TI Cup 2024 — 智能小车控制平台

基于 TI MSPM0G3507 (ARM Cortex-M0+) 的竞赛机器人小车项目，集成 IMU 姿态解算、双电机 PID 速度闭环、灰度循迹、OLED 显示及 UART 遥测。

## 硬件平台

| 组件 | 型号 | 接口 |
|---|---|---|
| MCU | MSPM0G3507 (Cortex-M0+, 80MHz) | — |
| IMU | BMI088 (六轴) | SPI |
| 电机驱动 | TB6612 (双 H 桥) | GPIO + PWM |
| 编码器 | 双电机霍尔编码器 | GPIO 定时器捕获 |
| 舵机 | 模拟/数字舵机 | PWM |
| 灰度传感器 | 8 路巡线模块 | GPIO 并行输入 |
| OLED | SSD1306 128×64 | I2C |
| 蜂鸣器 | 有源蜂鸣器 | GPIO |

## 软件架构

```
application/          # 上层算法（PID 控制、状态机、循迹解算）
modules/              # 功能模块（电机、舵机、IMU、OLED、按键、蜂鸣器、定时器）
bsp/                  # 外设抽象层（SPI、I2C、UART、Delay）
```

**模块自注册模式**：各模块在 `_Init()` 中自行调用 BSP 注册外设实例，不依赖 main.c 传入句柄。

**定时器回调系统**：

| 模块 | 定时器 | 周期 | 用途 |
|---|---|---|---|
| `pit_fast_tick` | TIMG12 | 1ms | 轻量快速任务（IMU 滴答、蜂鸣器计时） |
| `pit_control_tick` | TIMG0 | 20ms | 控制任务（电机速度闭环、按键扫描） |

每个 tick 支持最多 8 个回调，ISR 中按注册顺序执行。详见 [modules/pit_tick.md](modules/pit_tick.md)。

## 功能模块

- **IMU** — BMI088 驱动 + Mahony 互补滤波器，输出四元数、欧拉角、加速度；支持零偏自校准
- **Motor** — 双电机 PID 速度闭环，编码器测速反馈，前后台分离（ISR 计算、主循环调用）
- **Tracking** — 8 路灰度传感器加权插值算法，输出归一化黑线位置 [−1.0, +1.0]
- **State Machine** — 按键触发任务状态切换，回调注册机制
- **OLED** — I2C SSD1306 驱动，128×64 显示
- **PID** — 位置式 PID 控制器，积分/输出双限幅，支持在线调参与复位
- **Key** — 按键状态机，消抖与长按/短按识别
- **Servo** — PWM 占空比控制，支持角度映射
- **Buzzer** — 定时自动停止的蜂鸣器

## 构建与烧录

本项目使用 **CCS Theia**（TI Eclipse IDE），无 Makefile/CMake。

1. 用 CCS Theia 打开项目目录
2. 运行 SysConfig（`empty.syscfg`）生成 `ti_msp_dl_config.c/.h`
3. 点击 Build（Ctrl+B）编译，输出 `Debug/ti_cup_2024h.out`
4. 通过 J-Link 烧录到 LP-MSPM0G3507 LaunchPad

**关键依赖**：

- **编译器**: `tiarmclang` 4.0.4.LTS
- **SDK**: MSPM0-SDK 2.10.0.04
- **SysConfig**: 1.26.2
- **优化**: `-O0`（Debug），软浮点 ABI（Cortex-M0+ 无硬件 FPU）

## 引脚分配

见 `empty.syscfg`（SysConfig 图形化配置），主要外设：

| 外设 | 实例 | 关键引脚 |
|---|---|---|
| SPI (BMI088) | SPI0 | PB13 (ACCEL_CS), PB15 (GYRO_CS) |
| I2C (OLED) | I2C0 | PA11 (SDA), PA10 (SCL) |
| UART | UART0 | PA9 (TX), PA8 (RX) |
| PWM (电机) | TIMG2/6 | PB4/PB7 (M1), PA24/PA25 (M2) |
| PWM (舵机) | TIMG4 | PB22 |
| GPIO (灰度) | GPIOB | PB8–PB14 (IN1–IN7), PB0 (IN8) |
| GPIO (按键) | GPIOA | PA26 (KEY1), PA27 (KEY2), PA28 (KEY3) |

## 已知问题

- **左电机（Motor 1）测速异常**：编码器读数不稳定，PID 闭环暂不可用，见 `modules/motor.c` 中的 `@todo`
- **`TB6612_LimitPWM` 无符号比较**：`uint32_t period_count < 0` 为永假条件，属于死代码
- **主循环 `state` 变量未使用**：`main.c` 中 `state` 被 Key 回调写入，但主循环未读取
