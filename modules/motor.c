/* 本模块在使用tb6612电机驱动芯片时不需要修改
 * @todo 添加使用其他电机驱动芯片的接口
 */
#include "motor.h"

static volatile uint32_t g_encoder1_pulse;
static volatile uint32_t g_encoder2_pulse;
static volatile int32_t  g_encoder1_rpm;
static volatile int32_t  g_encoder2_rpm;

void Motor_Init(void)
{
    Motor_Stop();
    Motor_ResetEncoder();
    NVIC_EnableIRQ(GPIO_MOTORs_INT_IRQN);
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

void GROUP1_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(GPIOA))
    {
        case DL_GPIO_IIDX_DIO15:
            g_encoder1_pulse++;
            DL_GPIO_clearInterruptStatus(GPIOA, DL_GPIO_PIN_15);
            break;

        case DL_GPIO_IIDX_DIO7:
            g_encoder2_pulse++;
            DL_GPIO_clearInterruptStatus(GPIOA, DL_GPIO_PIN_7);
            break;

        default:
            break;
    }
}

uint32_t Motor_GetEncoder1Pulse(void)
{
    return g_encoder1_pulse;
}

uint32_t Motor_GetEncoder2Pulse(void)
{
    return g_encoder2_pulse;
}

int32_t Motor_GetEncoder1RPM(void)
{
    return g_encoder1_rpm;
}

int32_t Motor_GetEncoder2RPM(void)
{
    return g_encoder2_rpm;
}

void Motor_EncoderUpdate(void)
{
    static uint32_t last1, last2;
    uint32_t cur1, cur2;
    uint32_t diff1, diff2;

    cur1  = g_encoder1_pulse;
    cur2  = g_encoder2_pulse;

    diff1 = cur1 - last1;
    diff2 = cur2 - last2;

    last1 = cur1;
    last2 = cur2;

    g_encoder1_rpm = (int32_t)(diff1 * 100 / 11);
    g_encoder2_rpm = (int32_t)(diff2 * 100 / 11);
}

void Motor_ResetEncoder(void)
{
    g_encoder1_pulse = 0;
    g_encoder2_pulse = 0;
    g_encoder1_rpm   = 0;
    g_encoder2_rpm   = 0;
}
