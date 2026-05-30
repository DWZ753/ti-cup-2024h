#include "servo.h"

void Servo_Init(void)
{
    DL_Timer_startCounter(SERVO_PWM_INST);
    Servo_SetValue(0);
}

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

void Servo_SetValue(int32_t value)
{
    uint32_t duty;

    duty = SERVO_PULSE_CENTER_US + 
        (Servo_LimitValue(value) * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US) / 200);

    DL_Timer_setCaptureCompareValue(SERVO_PWM_INST, duty, SERVO_PWM_CHANNEL);
}
