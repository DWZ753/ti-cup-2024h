#ifndef IMU_H
#define IMU_H

#include "ti_msp_dl_config.h"
#include "spi.h"
#include "bmi088.h"
#include "mahony.h"

/**
 * @brief 初始化 IMU 子系统（SPI → BMI088 → Mahony）
 * @note  内部完成 SPI 注册、BMI088 软复位、Mahony 滤波器配置
 */
void IMU_Init(void);

/**
 * @brief 执行一次 IMU 采样 + 姿态解算
 * @note  读取加速度计和陀螺仪，送入 Mahony 滤波器完成一次迭代
 *         建议每 2ms 调用一次
 */
void IMU_Update(void);

/**
 * @brief 获取欧拉角
 * @param roll  输出横滚角 (°)
 * @param pitch 输出俯仰角 (°)
 * @param yaw   输出偏航角 (°)
 */
void IMU_GetEuler(float *roll, float *pitch, float *yaw);

/**
 * @brief 获取最新加速度计数据
 * @param accel 输出 [x, y, z]，单位 m/s²
 */
void IMU_GetAccel(float accel[3]);

/**
 * @brief 获取最新陀螺仪数据
 * @param gyro 输出 [x, y, z]，单位 rad/s
 */
void IMU_GetGyro(float gyro[3]);

/**
 * @brief 获取四元数
 * @param q0,q1,q2,q3 输出四元数各分量
 */
void IMU_GetQuaternion(float *q0, float *q1, float *q2, float *q3);

#endif
