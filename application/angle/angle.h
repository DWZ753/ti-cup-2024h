#ifndef __ANGLE_H__
#define __ANGLE_H__

#include "ti_msp_dl_config.h"
#include "pid.h"

/* ========== 可调参数（上赛道后在此集中调参） ========== */

// PID 参数
#define ANGLE_KP             2.0f    // 比例系数：每度偏差 → 舵机输出量
#define ANGLE_KI             0.02f   // 积分系数：消除长期稳态偏差
#define ANGLE_KD             0.0f    // 微分系数：先不加，阿克曼转向天然有阻尼
#define ANGLE_INTEGRAL_LIMIT 25.0f   // 积分上限，防止饱和
#define ANGLE_OUTPUT_LIMIT   50.0f   // 输出上限（舵量），不打满方向盘

// 滤波器与限幅
#define ANGLE_DEADBAND_DEG   2.0f    // 死区 ±2°，偏差在此范围内不修正
#define ANGLE_SLEW_MAX       15      // 舵机每次调用最大变化量

/* ========== API ========== */

/**
 * @brief 初始化角度保持控制器
 */
void Angle_Init(void);

/**
 * @brief 启用/关闭角度保持模式
 * @param enable  true=角度PID控制舵机, false=关闭(舵机由循迹模块接管)
 * @note  关闭时自动调用 Angle_Reset() 清理内部状态
 */
void Angle_Enable(bool enable);

/**
 * @brief 查询角度保持是否启用
 */
bool Angle_IsEnabled(void);

/**
 * @brief 设置目标航向角
 * @param heading_deg 目标偏航角（°），坐标系由 Mahony 滤波器定义
 */
void Angle_SetTarget(float heading_deg);

/**
 * @brief 执行一次角度 PID 计算并输出到舵机
 * @note  每 80ms 调用一次（ANGLE_PID_DT_MS）
 *        内部完成：读 yaw → 角度缠绕 → 死区 → PID → slew rate → Servo_SetValue
 */
void Angle_Compute(void);

/**
 * @brief 清零积分项、误差历史，舵机归零
 * @note  模式切换（循迹↔角度保持）或题目切换时调用
 */
void Angle_Reset(void);

/* ========== 遥测 Getter ========== */

float   Angle_GetTarget(void);
float   Angle_GetPIDOutput(void);
int32_t Angle_GetServoValue(void);

#endif
