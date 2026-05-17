#include "motor.h"


void Motor_Init(void)
{
    Motor_Stop();
}


uint32_t Motor_Speed_Limit(uint32_t speed)
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
    uint32_t duty = Motor_Speed_Limit(speed) * MOTOR_MAX_PWM_DUTY / MOTOR_SPEED_MAX;

    TB6612_A_Forward(duty);
    TB6612_B_Forward(duty);
}


void Motor_Backward(uint32_t speed)
{ 
    uint32_t duty = Motor_Speed_Limit(speed) * MOTOR_MAX_PWM_DUTY / MOTOR_SPEED_MAX;

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