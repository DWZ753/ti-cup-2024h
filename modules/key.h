#ifndef __KEY_H__
#define __KEY_H__

#include "ti_msp_dl_config.h"
#include "pit_control_tick.h"

#define KEY_PORT                GPIO_KEYs_PORT
// 按键数量定义
#define KEY_NUM                 4
// 消抖确认次数（连续检测到相同状态的次数）
#define KEY_DEBOUNCE_COUNT      3

// 按键有效电平枚举
typedef enum {
    KEY_ACTIVE_LOW = 0,   // 低电平有效（配置上拉电阻）
    KEY_ACTIVE_HIGH       // 高电平有效（配置下拉电阻）
} KeyActiveLevel_t;

typedef enum {
    KEY_STATE_IDLE = 0,      // 空闲状态
    KEY_STATE_PRESSED,       // 按下状态（消抖中）
    KEY_STATE_RELEASED       // 释放状态（已触发）
} KeyState_t;

typedef struct {
    uint32_t pin;                    // 按键引脚
    KeyActiveLevel_t active_level;  // 有效电平配置
    KeyState_t state;               // 当前状态
    uint8_t debounce_cnt;           // 消抖计数器
    uint8_t flag;                   // 按键触发标志
} Key_t;

// 外部按键数组声明
extern Key_t keys[KEY_NUM];

void Key_Init(void);
void Key_Scan(void);
uint8_t Key_GetFlag(uint8_t key_index);

#endif