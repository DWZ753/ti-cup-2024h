#include "main.h"
#include "application/pid.h"

static UART_Handle *uart_print;
static PID_Controller speed_pid;

static void Encoder_Display(void);

uint8_t state = 0;

int main(void)
{
    SYSCFG_DL_init();

    PIT_Custom_Tick_Init();
    PIT_Control_Tick_Init();
    Buzzer_Init();
    TB6612_Init();
    Motor_Init();
    Key_Init();
    Servo_Init();

    /* 注册 I2C0（OLED 用） */
    I2C_Config i2c_cfg = {
        .i2c          = I2C_OLED_INST,
        .sclPort      = GPIO_I2C_OLED_SCL_PORT,
        .sclPin       = GPIO_I2C_OLED_SCL_PIN,
        .sclIomux     = GPIO_I2C_OLED_IOMUX_SCL,
        .sclIomuxFunc = GPIO_I2C_OLED_IOMUX_SCL_FUNC,
        .sdaPort      = GPIO_I2C_OLED_SDA_PORT,
        .sdaPin       = GPIO_I2C_OLED_SDA_PIN,
        .sdaIomux     = GPIO_I2C_OLED_IOMUX_SDA,
        .sdaIomuxFunc = GPIO_I2C_OLED_IOMUX_SDA_FUNC,
        .syscfgInit   = SYSCFG_DL_I2C_OLED_init,
    };
    I2C_Handle *oled_i2c = I2C_Init(&i2c_cfg);
    OLED_Init(oled_i2c);

    /* 注册 UART0（PRINT）实例 */
    UART_Config uart_cfg = {
        .uart         = UART_PRINT_INST,
        .irqNum       = UART_PRINT_INT_IRQN,
        .dmaTxChanId  = UART0_DMA_TX_CHAN_ID,
        .dmaTxTrigger = PRINT_INST_DMA_TRIGGER,
    };
    uart_print = UART_Init(&uart_cfg);

    /* 初始化速度环 PID，输出限幅 = MOTOR_MAX_SPEED_MM_S */
    PID_Init(&speed_pid,
             3.0f,                  /* Kp */
             0.0f,                  /* Ki */
             0.00f,                 /* Kd */
             MOTOR_MAX_SPEED_MM_S,  /* 积分限幅 */
             MOTOR_MAX_SPEED_MM_S); /* 输出限幅：PID 输出为 mm/s */

    PID_SetTarget(&speed_pid, 1500.0f); /* 目标速度 mm/s */

    while (1)
    {
        /* PID 速度环：以两路编码器速度均值作为反馈 */
        // float cur_speed = (Motor_GetEncoder1Speed() + Motor_GetEncoder2Speed()) * 0.5f;
        float cur_speed = Motor_GetEncoder2Speed();
        float pid_out   = PID_Compute(&speed_pid, cur_speed);
        Motor_SetSpeed(pid_out);

        Encoder_Display();
        delay_ms(20);
    }
}

static void Encoder_Display(void)
{
    float sp1 = Motor_GetEncoder1Speed();
    float sp2 = Motor_GetEncoder2Speed();

    UART_Printf(uart_print, "%.1f,%.1f,%.1f\n",
                speed_pid.target, speed_pid.output, sp1);
}
