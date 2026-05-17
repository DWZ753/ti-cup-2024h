#include "buzzer.h"

static volatile uint32_t s_beep_time = 0;   // 蜂鸣器鸣叫时间
static bool s_is_active = false;            // 蜂鸣器是否激活


/**
 * @brief 启动蜂鸣器并设置鸣叫时长
 * 
 * 该函数用于激活蜂鸣器并指定其鸣叫的持续时间。调用后，蜂鸣器将根据设置的时长
 * 开始工作，由定时器回调函数 Buzzer_TickHandler 自动控制停止。
 * 
 * @param time_ms 蜂鸣器鸣叫的持续时间
 * 
 * @return 无
 * 
 * @note 在调用此函数前，必须先调用 Buzzer_Init 完成模块初始化
 * @note 如果蜂鸣器已在运行，再次调用此函数将重置鸣叫时间
 */
void Buzzer_Beep(uint32_t time_ms)
{
    s_beep_time = time_ms;
    s_is_active = true;
}


/**
 * @brief 停止蜂鸣器
 * 
 * 该函数用于立即停止蜂鸣器的鸣叫操作。调用后会清除激活状态标志，
 * 重置鸣叫时间计数器，并将GPIO引脚设置为高电平以关闭蜂鸣器。
 * 
 * @return 无
 * 
 * @note 此函数可由定时器回调自动调用（当鸣叫时间耗尽时），也可由外部主动调用
 * @note 调用此函数后，如需重新启动蜂鸣器，需再次调用 Buzzer_Start
 */
void Buzzer_Stop(void)
{
    s_is_active = false;
    s_beep_time = 0;
    DL_GPIO_setPins(GPIO_BUZZERs_PORT, GPIO_BUZZERs_GPIO_BUZZER_PIN);
}


/**
 * @brief 蜂鸣器定时器tick处理回调函数
 * 
 * 该函数由PIT自定义tick定时器周期性调用，用于控制蜂鸣器的鸣叫时长。
 * 在蜂鸣器激活状态下，每次调用会递减剩余鸣叫时间并控制GPIO引脚状态。
 * 当鸣叫时间耗尽时，自动停止蜂鸣器。
 * 
 * @return 无
 * 
 * @note 此函数为静态私有函数，仅作为定时器回调使用
 * 
 */
static void Buzzer_TickHandler(void)
{
    if (!s_is_active) return;

    if (s_beep_time > 0)
    {
        DL_GPIO_clearPins(GPIO_BUZZERs_PORT, GPIO_BUZZERs_GPIO_BUZZER_PIN);
        s_beep_time--;
    }
    else
    {
        Buzzer_Stop();
    }
}


/**
 * @brief 初始化蜂鸣器模块
 * 
 * @return 无
 * 
 * @note 此函数应在系统启动时调用一次，确保在调用Buzzer_Start之前完成初始化
 */
void Buzzer_Init(void)
{
    PIT_Custom_Tick_RegisterCallback(Buzzer_TickHandler);
}
