#ifndef PIT_CUSTOM_TICK_H
#define PIT_CUSTOM_TICK_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*PIT_Custom_Callback_t)(void);
void PIT_Custom_Tick_Init(void);
bool PIT_Custom_Tick_RegisterCallback(PIT_Custom_Callback_t callback);

#endif
