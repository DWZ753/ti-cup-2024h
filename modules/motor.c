/* 本模块在使用tb6612电机驱动芯片时不需要修改
 * @todo 添加使用其他电机驱动芯片的接口
 * @todo 左电机（即 motor1）的测速有bug，等待修复
 */
#include "motor.h"

// PWM 占空比下限：低于此值电机无法转动
#define MOTOR_MIN_DUTY 100

static volatile int32_t g_encoder1_pulse;
static volatile int32_t g_encoder2_pulse;
static volatile float   g_encoder1_rpm;
static volatile float   g_encoder2_rpm;
static volatile float   g_encoder1_speed;
static volatile float   g_encoder2_speed;

void Motor_Init(void)
{
    Motor_Stop();
    Motor_ResetEncoder();
    NVIC_EnableIRQ(GPIO_MOTORs_INT_IRQN);
    PIT_Control_Tick_RegisterCallback(Motor_TickHandler);
}

void Motor_SetSpeed(float speed_mm_s)
{
    float abs_speed = (speed_mm_s >= 0.0f) ? speed_mm_s : -speed_mm_s;

    uint32_t duty = (uint32_t)(abs_speed / MOTOR_MAX_SPEED_MM_S * MOTOR_MAX_PWM_DUTY);

    if (duty > MOTOR_MAX_PWM_DUTY)
        duty = MOTOR_MAX_PWM_DUTY;

    if (duty < MOTOR_MIN_DUTY)
    {
        Motor_Brake();
        return;
    }

    if (speed_mm_s >= 0.0f)
    {
        TB6612_A_Forward(duty);
        TB6612_B_Forward(duty);
    }
    else
    {
        TB6612_A_Backward(duty);
        TB6612_B_Backward(duty);
    }
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

static void encoder_isr(void)
{
    if (DL_GPIO_getEnabledInterruptStatus(MOTOR_ENCODER1_OUT_A_PORT, MOTOR_ENCODER1_OUT_A_PIN))
    {
        if (DL_GPIO_readPins(MOTOR_ENCODER1_OUT_B_PORT, MOTOR_ENCODER1_OUT_B_PIN) == 0)
            g_encoder1_pulse--;
        else
            g_encoder1_pulse++;
        DL_GPIO_clearInterruptStatus(MOTOR_ENCODER1_OUT_A_PORT, MOTOR_ENCODER1_OUT_A_PIN);
    }

    if (DL_GPIO_getEnabledInterruptStatus(MOTOR_ENCODER2_OUT_A_PORT, MOTOR_ENCODER2_OUT_A_PIN))
    {
        if (DL_GPIO_readPins(MOTOR_ENCODER2_OUT_B_PORT, MOTOR_ENCODER2_OUT_B_PIN) == 0)
            g_encoder2_pulse++;
        else
            g_encoder2_pulse--;
        DL_GPIO_clearInterruptStatus(MOTOR_ENCODER2_OUT_A_PORT, MOTOR_ENCODER2_OUT_A_PIN);
    }
}

void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
            encoder_isr();
            break;

        default:
            break;
    }
}

float Motor_GetEncoder1RPM(void)
{
    return g_encoder1_rpm;
}

float Motor_GetEncoder2RPM(void)
{
    return g_encoder2_rpm;
}

float Motor_GetEncoder1Speed(void)
{
    return g_encoder1_speed;
}

float Motor_GetEncoder2Speed(void)
{
    return g_encoder2_speed;
}

int32_t Motor_GetEncoder1Pulse(void)
{
    return g_encoder1_pulse;
}

int32_t Motor_GetEncoder2Pulse(void)
{
    return g_encoder2_pulse;
}

void Motor_TickHandler(void)
{
    static int32_t last1, last2;
    int32_t cur1, cur2;
    int32_t diff1, diff2;

    cur1  = g_encoder1_pulse;
    cur2  = g_encoder2_pulse;

    diff1 = cur1 - last1;
    diff2 = cur2 - last2;

    last1 = cur1;
    last2 = cur2;

    g_encoder1_rpm   = (float)diff1 * 100.0f / 11.0f;
    g_encoder2_rpm   = (float)diff2 * 100.0f / 11.0f;
    g_encoder1_speed = g_encoder1_rpm * WHEEL_CIRCUMFERENCE_MM / 60.0f;
    g_encoder2_speed = g_encoder2_rpm * WHEEL_CIRCUMFERENCE_MM / 60.0f;
}

void Motor_ResetEncoder(void)
{
    g_encoder1_pulse = 0;
    g_encoder2_pulse = 0;
    g_encoder1_rpm   = 0.0f;
    g_encoder2_rpm   = 0.0f;
    g_encoder1_speed = 0.0f;
    g_encoder2_speed = 0.0f;
}
