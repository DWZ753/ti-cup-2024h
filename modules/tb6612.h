#ifndef __TB6612_H__
#define __TB6612_H__ 

#include "ti_msp_dl_config.h"

// 这些宏定义根据实际情况在移植后修改
#define TB6612_PWM_TIMER 								PWM_MOTOR_INST
#define TB6612_PWM_A_PIN								GPIO_PWM_MOTOR_C0_IDX
#define TB6612_PWM_B_PIN								GPIO_PWM_MOTOR_C1_IDX
#define TB6612_GPIO_PORT								GPIO_MOTORs_PORT
#define TB6612_GPIO_AIN1_PIN							GPIO_MOTORs_GPIO_MOTOR1_IN1_PIN
#define TB6612_GPIO_AIN2_PIN							GPIO_MOTORs_GPIO_MOTOR1_IN2_PIN
#define TB6612_GPIO_BIN1_PIN							GPIO_MOTORs_GPIO_MOTOR2_IN1_PIN
#define TB6612_GPIO_BIN2_PIN							GPIO_MOTORs_GPIO_MOTOR2_IN2_PIN
#define TB6612_PWM_PERIOD_COUNT							10000

void TB6612_Init(void);
uint32_t TB6612_PWM_Period_Count_Limit(uint32_t period_count);

void TB6612_A_Forward(uint32_t duty);
void TB6612_A_Backward(uint32_t duty);
void TB6612_A_Brake(void);
void TB6612_A_Stop(void);

void TB6612_B_Forward(uint32_t duty);
void TB6612_B_Backward(uint32_t duty);
void TB6612_B_Brake(void);
void TB6612_B_Stop(void);

#endif