#include "delay.h"

/**
 * @brief 毫秒级延时函数
 * 
 * 通过消耗CPU时钟周期实现精确的毫秒级延时。
 * 
 * @param ms 延时的毫秒数，取值范围: 0 ~ ULONG_MAX
 * 
 * @return 无
 */
void delay_ms(unsigned long ms)
{
    delay_cycles(ms * (CPUCLK_FREQ / 1000UL));
}
