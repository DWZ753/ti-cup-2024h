#include "servo.h"

/**
 * @brief 舵机初始化函数
 * 
 * 启动PWM定时器计数器并将舵机设置到中心位置。
 * @return 无
 * @note 此函数应在系统初始化阶段调用，确保舵机PWM信号正常输出。
 */
void Servo_Init(void)
{
    DL_Timer_startCounter(SERVO_PWM_INST);
    Servo_SetValue(0);
}

/**
 * @brief 限制舵机控制值在有效范围内
 * 
 * 将输入的舵机控制值限制在 [-100, 100] 的范围内，防止超出舵机的有效控制区间。
 * 
 * @param value 需要限制的舵机控制值
 * @return 限制后的值
 */
int32_t Servo_LimitValue(int32_t value)
{
    if (value > 100)
    {
        value = 100;
    }
    if (value < -100)
    {
        value = -100;
    }

    return value;
}

/**
 * @brief 设置舵机输出值
 * 
 * 输入值会被限制在有效范围内，然后线性映射到舵机的脉冲宽度范围。
 * @param value 舵机控制值，范围为-100到100的整数
 * 
 *              - 正值表示正向偏转
 * 
 *              - 负值表示反向偏转
 * 
 *              - 0表示中位
 * @return 无
 */
void Servo_SetValue(int32_t value)
{
    uint32_t duty;

    duty = SERVO_PULSE_CENTER_US + 
        (Servo_LimitValue(value) * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US) / 200);

    DL_Timer_setCaptureCompareValue(SERVO_PWM_INST, duty, SERVO_PWM_CHANNEL);
}
