#include "tb6612.h"


/**
 * @brief 初始化TB6612电机驱动模块

 * 启动TB6612 PWM定时器计数器，使能PWM信号输出以控制电机
 */
void TB6612_Init(void)
{
   DL_Timer_startCounter(TB6612_PWM_TIMER);
}


/**
 * @brief 限制TB6612 PWM周期计数值在有效范围内
 * 
 * 该函数确保PWM周期计数值不超过最大允许值且不小于最小值，
 * 防止无效的PWM配置导致硬件异常。
 * 
 * @param period_count 输入的PWM周期计数值
 * @return uint32_t 限制后的PWM周期计数值，范围在[0, TB6612_PWM_PERIOD_COUNT]之间
 */
uint32_t TB6612_LimitPWM(uint32_t period_count)
{
    if (period_count > TB6612_PWM_PERIOD_COUNT)
    {
        period_count = TB6612_PWM_PERIOD_COUNT;
    }
    if (period_count < 0)
    {
        period_count = 0;
    }

    return period_count;
}


/**
 * @brief 控制A电机正转
 * 
 * 该函数设置A电机的控制引脚为正向驱动状态，并配置PWM占空比以控制电机转速。
 * 
 * AIN1置高电平，AIN2置低电平，使电机正向旋转。
 * 
 * @param duty PWM占空比值，范围在[0, TB6612_PWM_PERIOD_COUNT]之间，值越大转速越快
 */
void TB6612_A_Forward(uint32_t duty)
{
    duty = TB6612_LimitPWM(duty);

    DL_GPIO_setPins(TB6612_GPIO_PORT, TB6612_GPIO_AIN1_PIN);
    DL_GPIO_clearPins(TB6612_GPIO_PORT, TB6612_GPIO_AIN2_PIN);
    DL_Timer_setCaptureCompareValue(TB6612_PWM_TIMER, duty, TB6612_PWM_A_PIN);
}


/**
 * @brief 控制A电机反转
 * 
 * 该函数设置A电机的控制引脚为反向驱动状态，并配置PWM占空比以控制电机转速。
 * 
 * AIN1置低电平，AIN2置高电平，使电机反向旋转。
 * 
 * @param duty PWM占空比值，范围在[0, TB6612_PWM_PERIOD_COUNT]之间，值越大转速越快
 */
void TB6612_A_Backward(uint32_t duty)
{ 
    duty = TB6612_LimitPWM(duty);

    DL_GPIO_clearPins(TB6612_GPIO_PORT, TB6612_GPIO_AIN1_PIN);
    DL_GPIO_setPins(TB6612_GPIO_PORT, TB6612_GPIO_AIN2_PIN);
    DL_Timer_setCaptureCompareValue(TB6612_PWM_TIMER, duty, TB6612_PWM_A_PIN);
}


/**
 * @brief 控制A电机制动
 * 
 * 该函数设置A电机的两个控制引脚均为高电平，使电机制动。
 */
void TB6612_A_Brake(void)
{
    DL_GPIO_setPins(TB6612_GPIO_PORT, TB6612_GPIO_AIN1_PIN | TB6612_GPIO_AIN2_PIN);
}


/**
 * @brief 停止A电机
 * 
 * 该函数将A电机的两个控制引脚都置低电平，使电机滑行。
 */
void TB6612_A_Stop(void)
{
    DL_GPIO_clearPins(TB6612_GPIO_PORT, TB6612_GPIO_AIN1_PIN | TB6612_GPIO_AIN2_PIN);
}


/**
 * @brief 控制B电机正转
 * 
 * 该函数设置B电机的控制引脚为正向驱动状态，并配置PWM占空比以控制电机转速。
 * 
 * BIN1置高电平，BIN2置低电平，使电机正向旋转。
 * 
 * @param duty PWM占空比值，范围在[0, TB6612_PWM_PERIOD_COUNT]之间，值越大转速越快
 */
void TB6612_B_Forward(uint32_t duty)
{ 
    duty = TB6612_LimitPWM(duty);

    DL_GPIO_setPins(TB6612_GPIO_PORT, TB6612_GPIO_BIN1_PIN);
    DL_GPIO_clearPins(TB6612_GPIO_PORT, TB6612_GPIO_BIN2_PIN);
    DL_Timer_setCaptureCompareValue(TB6612_PWM_TIMER, duty, TB6612_PWM_B_PIN);
}


/**
 * @brief 控制B电机反转
 * 
 * 该函数设置B电机的控制引脚为反向驱动状态，并配置PWM占空比以控制电机转速。
 * 
 * BIN1置低电平，BIN2置高电平，使电机反向旋转。
 * 
 * @param duty PWM占空比值，范围在[0, TB6612_PWM_PERIOD_COUNT]之间，值越大转速越快
 */
void TB6612_B_Backward(uint32_t duty)
{ 
    duty = TB6612_LimitPWM(duty);

    DL_GPIO_clearPins(TB6612_GPIO_PORT, TB6612_GPIO_BIN1_PIN);
    DL_GPIO_setPins(TB6612_GPIO_PORT, TB6612_GPIO_BIN2_PIN);
    DL_Timer_setCaptureCompareValue(TB6612_PWM_TIMER, duty, TB6612_PWM_B_PIN);
}


/**
 * @brief 停止B电机
 * 
 * 该函数将B电机的两个控制引脚都置低电平，使电机制动。
 */
void TB6612_B_Brake(void)
{
    DL_GPIO_setPins(TB6612_GPIO_PORT, TB6612_GPIO_BIN1_PIN | TB6612_GPIO_BIN2_PIN);
}


/**
 * @brief 停止B电机
 * 
 * 该函数将B电机的两个控制引脚都置低电平，使电机滑行。
 */
void TB6612_B_Stop(void)
{
    DL_GPIO_clearPins(TB6612_GPIO_PORT, TB6612_GPIO_BIN1_PIN | TB6612_GPIO_BIN2_PIN);
}
