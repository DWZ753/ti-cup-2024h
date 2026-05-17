/* 本模块在使用tb6612芯片时不需要修改
 * @todo : 添加使用其他电机驱动芯片的接口
 */
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "ti_msp_dl_config.h"

#include "tb6612.h"

#define MOTOR_MAX_PWM_DUTY (TB6612_PWM_PERIOD_COUNT - 1)
#define MOTOR_SPEED_MAX 100
#define MOTOR_SPEED_MIN 0

void Motor_Init(void);
uint32_t Motor_Speed_Limit(uint32_t speed);
void Motor_Forward(uint32_t speed);
void Motor_Backward(uint32_t speed);
void Motor_Brake(void);
void Motor_Stop(void);

#endif
