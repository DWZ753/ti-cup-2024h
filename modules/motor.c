/* 本模块在使用tb6612电机驱动芯片时不需要修改
 * @todo 添加使用其他电机驱动芯片的接口
 */
#include "motor.h"

/**
 * @brief 电机初始化函数
 * 
 * 初始化电机模块，将电机设置为停止状态。
 * @return 无
 */
void Motor_Init(void)
{
    Motor_Stop();
}


uint32_t Motor_LimitSpeed(uint32_t speed)
{
    if (speed > MOTOR_SPEED_MAX)
    {
        speed = MOTOR_SPEED_MAX;
    }
    if (speed < MOTOR_SPEED_MIN)
    {
        speed = MOTOR_SPEED_MIN;
    }

    return speed;
}


void Motor_Forward(uint32_t speed)
{
    uint32_t duty = Motor_LimitSpeed(speed) * MOTOR_MAX_PWM_DUTY / MOTOR_SPEED_MAX;

    TB6612_A_Forward(duty);
    TB6612_B_Forward(duty);
}


void Motor_Backward(uint32_t speed)
{ 
    uint32_t duty = Motor_LimitSpeed(speed) * MOTOR_MAX_PWM_DUTY / MOTOR_SPEED_MAX;

    TB6612_A_Backward(duty);
    TB6612_B_Backward(duty);
}


void Motor_Brake(void)
{
    TB6612_A_Brake();
    TB6612_B_Brake();
}


void Motor_Stop(void)
{
    TB6612_A_Stop();
    TB6612_B_Stop();
}