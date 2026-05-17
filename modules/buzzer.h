#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "ti_msp_dl_config.h"
#include "pit_custom_tick.h"

void Buzzer_Beep(uint32_t time_ms);
void Buzzer_Stop(void);
void Buzzer_Init(void);

#endif