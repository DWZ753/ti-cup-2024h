#include "main.h"

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

    Motor_Forward(50);

    while (1)
    {
        // Buzzer_Beep(100);
        Encoder_Display();
        delay_ms(100);
    }
}

static void Encoder_Display(void)
{
    float speed1 = Motor_GetEncoder1Speed();
    float speed2 = Motor_GetEncoder2Speed();
    float rpm1   = Motor_GetEncoder1RPM();
    float rpm2   = Motor_GetEncoder2RPM();

    int32_t rpm1_x10   = (int32_t)(rpm1 * 10.0f);
    int32_t rpm2_x10   = (int32_t)(rpm2 * 10.0f);
    int32_t speed1_x10 = (int32_t)(speed1 * 10.0f);
    int32_t speed2_x10 = (int32_t)(speed2 * 10.0f);

    char buf[32];

    {
        int abs10 = rpm1_x10 >= 0 ? rpm1_x10 : -rpm1_x10;
        sprintf(buf, "M1 RPM: %c%4d.%d", rpm1_x10 < 0 ? '-' : ' ',
                (int)(abs10 / 10), (int)(abs10 % 10));
        OLED_ShowString(0, 0, (uint8_t *)buf, 16);
    }
    {
        int abs10 = rpm2_x10 >= 0 ? rpm2_x10 : -rpm2_x10;
        sprintf(buf, "M2 RPM: %c%4d.%d", rpm2_x10 < 0 ? '-' : ' ',
                (int)(abs10 / 10), (int)(abs10 % 10));
        OLED_ShowString(0, 2, (uint8_t *)buf, 16);
    }
    {
        int abs10 = speed1_x10 >= 0 ? speed1_x10 : -speed1_x10;
        sprintf(buf, "M1: %c%4d.%d mm/s", speed1_x10 < 0 ? '-' : ' ',
                (int)(abs10 / 10), (int)(abs10 % 10));
        OLED_ShowString(0, 4, (uint8_t *)buf, 16);
    }
    {
        int abs10 = speed2_x10 >= 0 ? speed2_x10 : -speed2_x10;
        sprintf(buf, "M2: %c%4d.%d mm/s", speed2_x10 < 0 ? '-' : ' ',
                (int)(abs10 / 10), (int)(abs10 % 10));
        OLED_ShowString(0, 6, (uint8_t *)buf, 16);
    }
}
