#include "state_machine.h"

static QuestionState_t s_current_state = STATE_IDLE;

static void Key_Handler(uint8_t key_index)
{
    // 按键 1-4 映射到 TASK1-TASK4
    s_current_state = (QuestionState_t)(key_index + 1);

    // LED 翻转作为按键反馈
    DL_GPIO_togglePins(GPIO_LEDs_PORT, GPIO_LEDs_GPIO_LED_PIN);
}

void StateMachine_Init(void)
{
    Key_RegisterCallback(Key_Handler);
}

QuestionState_t StateMachine_GetState(void)
{
    return s_current_state;
}
