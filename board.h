#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdio.h>
#include "ti_msp_dl_config.h"
#include "pit_custom_tick.h"
#include "pit_control_tick.h"
#include "buzzer.h"
#include "tb6612.h"
#include "motor.h"
#include "key.h"
#include "grayscale.h"
#include "servo.h"
#include "uart.h"
#include "imu.h"
#include "oled.h"

/**
 * @brief 板级初始化，按依赖顺序调用所有模块的 Init
 * @note  调用前需确保 SYSCFG_DL_init() 已执行
 */
void Board_Init(void);

/**
 * @brief 获取系统 1ms 滴答计数
 * @return 自启动以来的毫秒数
 */
uint32_t Board_GetTickMs(void);

/**
 * @brief 获取 UART 打印句柄
 * @return UART 句柄指针
 */
UART_Handle* Board_GetUART(void);

#endif
