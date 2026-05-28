#include "board.h"

/* ========== 静态变量 ========== */

static volatile uint32_t imu_ticks;
static UART_Handle      *uart_print;

/* ========== 内部回调 ========== */

static void imu_tick_cb(void)
{
    imu_ticks++;
}

/* ========== 公共 API ========== */

void Board_Init(void)
{
    // 定时器
    PIT_Custom_Tick_Init();
    PIT_Custom_Tick_RegisterCallback(imu_tick_cb);
    PIT_Control_Tick_Init();

    // 执行器
    Buzzer_Init();
    TB6612_Init();
    Motor_Init();
    Servo_Init();

    // 输入
    Key_Init();

    /* 通信 */
    UART_Config uart_cfg = {
        .uart         = UART_PRINT_INST,
        .irqNum       = UART_PRINT_INT_IRQN,
        .dmaTxChanId  = UART0_DMA_TX_CHAN_ID,
        .dmaTxTrigger = PRINT_INST_DMA_TRIGGER,
    };
    uart_print = UART_Init(&uart_cfg);

    // 传感器
    IMU_Init();

    // 显示
    OLED_Init();
}

uint32_t Board_GetTickMs(void)
{
    return imu_ticks;
}

UART_Handle* Board_GetUART(void)
{
    return uart_print;
}
