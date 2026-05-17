#ifndef PIT_CONTROL_TICK_H
#define PIT_CONTROL_TICK_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*PIT_Control_Callback_t)(void);
void PIT_Control_Tick_Init(void);
bool PIT_Control_Tick_RegisterCallback(PIT_Control_Callback_t callback);

#endif
