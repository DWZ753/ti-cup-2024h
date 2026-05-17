#include "servo.h"

/**
 * @brief 初始化舵机模块
 *
 * 设置舵机到中位(90°)，然后启动PWM定时器计数器。
 * 应在SYSCFG_DL_init()之后调用。
 *
 * @note 先设置中位再启动定时器，避免上电瞬间舵机乱摆
 */
void Servo_Init(void)
{
    DL_Timer_setCaptureCompareValue(SERVO_PWM_INST,
        SERVO_US_TO_COUNTS(SERVO_PULSE_CENTER_US),
        SERVO_PWM_CHANNEL);
    DL_Timer_startCounter(SERVO_PWM_INST);
}

/**
 * @brief 将角度值限制在有效范围内
 *
 * 输入超出[SERVO_ANGLE_MIN, SERVO_ANGLE_MAX]范围的角度值时，
 * 自动钳位到最近的有效边界值。
 *
 * @param angle 输入的角度值(°)
 * @return uint32_t 限制后的角度值
 */
uint32_t Servo_LimitAngle(uint32_t angle)
{
    if (angle > SERVO_ANGLE_MAX)
    {
        angle = SERVO_ANGLE_MAX;
    }
    if (angle < SERVO_ANGLE_MIN)
    {
        angle = SERVO_ANGLE_MIN;
    }

    return angle;
}

/**
 * @brief 设置舵机角度
 *
 * 将角度线性映射为PWM脉宽并输出至舵机。
 * 映射关系：0°→0.5ms脉宽，90°→1.5ms脉宽（中位），180°→2.5ms脉宽。
 *
 * 阿克曼转向用法：
 *   - 90° = 车轮回正，直行
 *   - < 90° = 左转（推荐60°-90°，即左偏0-30°）
 *   - > 90° = 右转（推荐90°-120°，即右偏0-30°）
 *
 * @param angle 目标角度(°)，范围[0, 180]，超出部分自动钳位
 */
void Servo_SetAngle(uint32_t angle)
{
    uint32_t pulse_us;
    uint32_t pulse;

    angle = Servo_LimitAngle(angle);

    // 角度 → 微秒脉宽：us = 500 + angle * 2000 / 180
    pulse_us = SERVO_PULSE_MIN_US
               + angle * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US)
                     / SERVO_ANGLE_MAX;

    /* 微秒脉宽 → 定时器计数值 */
    pulse = SERVO_US_TO_COUNTS(pulse_us);

    DL_Timer_setCaptureCompareValue(SERVO_PWM_INST, pulse, SERVO_PWM_CHANNEL);
}
