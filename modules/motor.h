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

#define MOTOR_SPEED_MAX 100
#define MOTOR_SPEED_MIN 0

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
 * @brief 编码器转速更新（需在周期性中断中调用）
 * @note 根据两次调用间的脉冲差值计算 RPM 和线速度
 */
void Motor_TickHandler(void);

/**
 * @brief 清零编码器脉冲计数
 */
void Motor_ResetEncoder(void);

#endif
