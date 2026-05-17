#ifndef __SERVO_H__
#define __SERVO_H__

#include "ti_msp_dl_config.h"

/* ========== 硬件映射宏（移植到其他MCU时修改此部分） ========== */
#define SERVO_PWM_INST              PWM_SERVO_INST              /* 定时器外设实例         */
#define SERVO_PWM_CHANNEL           GPIO_PWM_SERVO_C0_IDX       /* 捕获比较通道索引       */
#define SERVO_PWM_CLK_FREQ          PWM_SERVO_INST_CLK_FREQ     /* PWM定时器时钟频率(Hz)  */

/* ========== 舵机参数宏（更换舵机型号时修改此部分） ========== */
#define SERVO_ANGLE_MAX             180     /* 最大角度(°)               */
#define SERVO_ANGLE_MIN             0       /* 最小角度(°)               */
#define SERVO_ANGLE_CENTER          90      /* 中位角度(°)，阿克曼直行    */
#define SERVO_PULSE_MIN_US          500     /* 0° 对应脉宽(us)           */
#define SERVO_PULSE_MAX_US          2500    /* 180°对应脉宽(us)          */
#define SERVO_PULSE_CENTER_US       1500    /* 90°对应脉宽(us)           */

/* ========== 微秒脉宽转定时器计数值 ========== */
#define SERVO_US_TO_COUNTS(us)      ((uint32_t)(us) * (SERVO_PWM_CLK_FREQ / 1000000UL))

void Servo_Init(void);
uint32_t Servo_LimitAngle(uint32_t angle);
void Servo_SetAngle(uint32_t angle);

#endif
