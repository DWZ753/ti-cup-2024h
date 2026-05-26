#include "main.h"

static UART_Handle *uart_print;
static volatile uint32_t imu_ticks;  /* 1ms PIT 滴答计数 */

static void imu_tick_cb(void)
{
    imu_ticks++;
}

int state;

int main(void)
{
    SYSCFG_DL_init();

    PIT_Custom_Tick_Init();
    PIT_Custom_Tick_RegisterCallback(imu_tick_cb);
    PIT_Control_Tick_Init();
    Buzzer_Init();
    TB6612_Init();
    Motor_Init();
    Key_Init();
    Servo_Init();

    /* 注册 I2C0（OLED 用） */
    // I2C_Config i2c_cfg = {
    //     .i2c          = I2C_OLED_INST,
    //     .sclPort      = GPIO_I2C_OLED_SCL_PORT,
    //     .sclPin       = GPIO_I2C_OLED_SCL_PIN,
    //     .sclIomux     = GPIO_I2C_OLED_IOMUX_SCL,
    //     .sclIomuxFunc = GPIO_I2C_OLED_IOMUX_SCL_FUNC,
    //     .sdaPort      = GPIO_I2C_OLED_SDA_PORT,
    //     .sdaPin       = GPIO_I2C_OLED_SDA_PIN,
    //     .sdaIomux     = GPIO_I2C_OLED_IOMUX_SDA,
    //     .sdaIomuxFunc = GPIO_I2C_OLED_IOMUX_SDA_FUNC,
    //     .syscfgInit   = SYSCFG_DL_I2C_OLED_init,
    // };
    // I2C_Handle *oled_i2c = I2C_Init(&i2c_cfg);
    // OLED_Init(oled_i2c);

    /* 注册 UART0（PRINT） */
    UART_Config uart_cfg = {
        .uart         = UART_PRINT_INST,
        .irqNum       = UART_PRINT_INT_IRQN,
        .dmaTxChanId  = UART0_DMA_TX_CHAN_ID,
        .dmaTxTrigger = PRINT_INST_DMA_TRIGGER,
    };
    uart_print = UART_Init(&uart_cfg);

    /* 初始化 BMI088 */
    BMI088_Config bmi_cfg = {
        .spi         = SPI_BMI088_INST,
        .csAccelPort = GPIO_BMI088_PORT,
        .csAccelPin  = GPIO_BMI088_CS1_PIN,
        .csGyroPort  = GPIO_BMI088_PORT,
        .csGyroPin   = GPIO_BMI088_CS2_PIN,
    };
    BMI088_Init(&bmi_cfg);

    /* 初始化 Mahony 滤波器 */
    struct MAHONY_FILTER_t mahony;
    mahony_init(&mahony, 15.0f, 0.002f, 0.002f);

    float    accel[3] = {0}, gyro[3] = {0};
    Axis3f   gyro_axis, acc_axis;
    uint32_t last_imu    = 0;
    uint32_t last_output = 0;

    while (1)
    {
        uint32_t now = imu_ticks;

        /* Mahony更新 */
        if (now - last_imu >= 2) {
            last_imu = now;

            BMI088_ReadAccel(accel);   /* m/s² */
            BMI088_ReadGyro(gyro);     /* rad/s */

            acc_axis.x  = accel[0]; acc_axis.y  = accel[1]; acc_axis.z  = accel[2];
            gyro_axis.x = gyro[0];  gyro_axis.y = gyro[1];  gyro_axis.z = gyro[2];

            mahony_input(&mahony, gyro_axis, acc_axis);
            mahony_update(&mahony);
            mahony_output(&mahony);
        }

        /* 串口输出 */
        if (now - last_output >= 10) {
            last_output = now;
            UART_Printf(uart_print, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                        mahony.q0, mahony.q1, mahony.q2, mahony.q3,
                        mahony.pitch, mahony.roll, mahony.yaw,
                        accel[0], accel[1], accel[2]);
        }
    }
}
