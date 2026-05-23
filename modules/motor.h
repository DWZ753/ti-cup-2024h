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

/* 编码器参数：11线 × 1:30减速比 = 330 脉冲/输出轴转 */
#define ENCODER_PPR                   11
#define ENCODER_GEAR_RATIO            30
#define ENCODER_PULSES_PER_OUTPUT_REV (ENCODER_PPR * ENCODER_GEAR_RATIO)

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

/**
 * @brief 获取电机1编码器累计脉冲数
 * @return 脉冲计数值
 */
uint32_t Motor_GetEncoder1Pulse(void);

/**
 * @brief 获取电机2编码器累计脉冲数
 * @return 脉冲计数值
 */
uint32_t Motor_GetEncoder2Pulse(void);

/**
 * @brief 获取电机1当前转速（RPM）
 * @return 转速值（转/分钟），正转时为正值，反转时需要结合方向判断
 * @note 需周期性调用 Motor_EncoderUpdate 来更新转速计算
 */
int32_t Motor_GetEncoder1RPM(void);

/**
 * @brief 获取电机2当前转速（RPM）
 * @return 转速值（转/分钟）
 * @note 需周期性调用 Motor_EncoderUpdate 来更新转速计算
 */
int32_t Motor_GetEncoder2RPM(void);

/**
 * @brief 编码器转速更新（需在周期性中断中调用，如 PIT 20ms 回调）
 * @note 根据两次调用间的脉冲差值计算 RPM
 */
void Motor_EncoderUpdate(void);

/**
 * @brief 清零编码器脉冲计数
 */
void Motor_ResetEncoder(void);

#endif
