#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "ti_msp_dl_config.h"
#include "key.h"
#include <stdint.h>

/* 任务状态枚举 */
typedef enum {
    STATE_IDLE = 0,
    STATE_TASK1,
    STATE_TASK2, 
    STATE_TASK3,
    STATE_TASK4
} QuestionState_t;

/**
 * @brief 初始化状态机，注册按键回调
 */
void StateMachine_Init(void);

/**
 * @brief 获取当前状态
 */
QuestionState_t StateMachine_GetState(void);

#endif
