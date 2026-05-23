#include "empty.h"

static UART_Handle *uart_print;

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

    /* 每 20ms 更新一次编码器转速计算 */
    PIT_Control_Tick_RegisterCallback(Motor_EncoderUpdate);

    Motor_Forward(30);

    while (1)
    {
        // Buzzer_Beep(100);
        Encoder_Display();
        delay_ms(100);
    }
}

static void Encoder_Display(void)
{
    uint32_t pulse1 = Motor_GetEncoder1Pulse();
    uint32_t pulse2 = Motor_GetEncoder2Pulse();
    int32_t  rpm1   = Motor_GetEncoder1RPM();
    int32_t  rpm2   = Motor_GetEncoder2RPM();

    OLED_ShowString(0, 0, (uint8_t *)"M1 Pul:", 16);
    OLED_ShowNum(64, 0, pulse1, 5, 16);

    OLED_ShowString(0, 2, (uint8_t *)"M1 RPM:", 16);
    OLED_ShowNum(64, 2, (uint32_t)rpm1, 5, 16);

    OLED_ShowString(0, 4, (uint8_t *)"M2 Pul:", 16);
    OLED_ShowNum(64, 4, pulse2, 5, 16);

    OLED_ShowString(0, 6, (uint8_t *)"M2 RPM:", 16);
    OLED_ShowNum(64, 6, (uint32_t)rpm2, 5, 16);
}
