/*
 * 本模块在使用 TB6612 电机驱动芯片时不需要修改
 * @todo 添加使用其他电机驱动芯片的接口
 */
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "ti_msp_dl_config.h"
#include "tb6612.h"

#define MOTOR_MAX_PWM_DUTY (TB6612_PWM_PERIOD_COUNT - 1)
#define MOTOR_SPEED_MAX 100
#define MOTOR_SPEED_MIN 0

/**
 * @brief 电机初始化，将电机置于停止状态
 */
void Motor_Init(void);

/**
 * @brief 将电机速度值限制在有效范围 [MOTOR_SPEED_MIN, MOTOR_SPEED_MAX] 内
 * @param speed 原始速度值
 * @return 限制后的速度值
 */
uint32_t Motor_LimitSpeed(uint32_t speed);

/**
 * @brief 两路电机同时正转
 * @param speed 速度值 [0, MOTOR_SPEED_MAX]
 */
void Motor_Forward(uint32_t speed);

/**
 * @brief 两路电机同时反转
 * @param speed 速度值 [0, MOTOR_SPEED_MAX]
 */
void Motor_Backward(uint32_t speed);

/**
 * @brief 两路电机同时制动
 */
void Motor_Brake(void);

/**
 * @brief 两路电机同时滑行停止
 */
void Motor_Stop(void);

#endif
