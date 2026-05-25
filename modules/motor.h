/*
 * 本模块在使用 TB6612 电机驱动芯片时不需要修改
 * @todo 添加使用其他电机驱动芯片的接口
 */
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "ti_msp_dl_config.h"
#include "pit_control_tick.h"
#include "tb6612.h"

#define MOTOR_MAX_PWM_DUTY (TB6612_PWM_PERIOD_COUNT - 1)

/* 电机最高转速 RPM（实测） */
#define MOTOR_MAX_RPM 300.0f

/* 编码器参数：11线 × 1:30减速比 = 330 脉冲/输出轴转 */
#define MOTOR_ENCODER_PPR                   11
#define MOTOR_ENCODER_GEAR_RATIO            30
#define MOTOR_ENCODER_PULSES_PER_OUTPUT_REV (MOTOR_ENCODER_PPR * MOTOR_ENCODER_GEAR_RATIO)

/* 编码器 A 相引脚宏定义 */
#define MOTOR_ENCODER1_OUT_A_PORT GPIO_MOTORs_GPIO_MOTOR1_OUT_A_PORT
#define MOTOR_ENCODER1_OUT_A_PIN  GPIO_MOTORs_GPIO_MOTOR1_OUT_A_PIN
#define MOTOR_ENCODER1_OUT_A_IIDX GPIO_MOTORs_GPIO_MOTOR1_OUT_A_IIDX
#define MOTOR_ENCODER2_OUT_A_PORT GPIO_MOTORs_GPIO_MOTOR2_OUT_A_PORT
#define MOTOR_ENCODER2_OUT_A_PIN  GPIO_MOTORs_GPIO_MOTOR2_OUT_A_PIN
#define MOTOR_ENCODER2_OUT_A_IIDX GPIO_MOTORs_GPIO_MOTOR2_OUT_A_IIDX

/* 编码器 B 相引脚宏定义（仅读电平，不触发中断） */
#define MOTOR_ENCODER1_OUT_B_PORT GPIO_MOTORs_GPIO_MOTOR1_OUT_B_PORT
#define MOTOR_ENCODER1_OUT_B_PIN  GPIO_MOTORs_GPIO_MOTOR1_OUT_B_PIN
#define MOTOR_ENCODER2_OUT_B_PORT GPIO_MOTORs_GPIO_MOTOR2_OUT_B_PORT
#define MOTOR_ENCODER2_OUT_B_PIN  GPIO_MOTORs_GPIO_MOTOR2_OUT_B_PIN

/* 轮子参数 */
#define WHEEL_RADIUS_MM         68.0f
#define WHEEL_CIRCUMFERENCE_MM  (2.0f * 3.1415926f * WHEEL_RADIUS_MM)

/* 最大线速度 mm/s = 最高转速 × 周长 / 60 */
#define MOTOR_MAX_SPEED_MM_S    (MOTOR_MAX_RPM / 60.0f * WHEEL_CIRCUMFERENCE_MM)

/**
 * @brief 电机初始化，将电机置于停止状态
 */
void Motor_Init(void);

/**
 * @brief 设置电机目标线速度（统一接口，正值前进 / 负值后退）
 * @param speed_mm_s 目标线速度 mm/s，范围 [-MOTOR_MAX_SPEED_MM_S, MOTOR_MAX_SPEED_MM_S]
 */
void Motor_SetSpeed(float speed_mm_s);

/**
 * @brief 两路电机同时制动
 */
void Motor_Brake(void);

/**
 * @brief 两路电机同时滑行停止
 */
void Motor_Stop(void);

/**
 * @brief 获取电机1当前转速
 * @return 转速值（RPM）
 */
float Motor_GetEncoder1RPM(void);

/**
 * @brief 获取电机2当前转速
 * @return 转速值（RPM）
 */
float Motor_GetEncoder2RPM(void);

/**
 * @brief 获取电机1当前线速度
 * @return 线速度（mm/s）
 */
float Motor_GetEncoder1Speed(void);

/**
 * @brief 获取电机2当前线速度
 * @return 线速度（mm/s）
 */
float Motor_GetEncoder2Speed(void);

/**
 * @brief 获取电机1编码器原始脉冲计数
 * @return 脉冲计数值
 */
int32_t Motor_GetEncoder1Pulse(void);

/**
 * @brief 获取电机2编码器原始脉冲计数
 * @return 脉冲计数值
 */
int32_t Motor_GetEncoder2Pulse(void);

/**
 * @brief 编码器转速更新（在 PIT 控制中断中调用）
 * @note 根据两次调用间的脉冲差值计算 RPM 和线速度，调用周期 20ms
 */
void Motor_TickHandler(void);

/**
 * @brief 清零编码器脉冲计数
 */
void Motor_ResetEncoder(void);

#endif
