#include "bmi088.h"
#include "delay.h"

/* ========== 寄存器地址 ========== */
#define ACCEL_CHIP_ID       0x00
#define ACCEL_X_LSB         0x12
#define ACCEL_PWR_CTRL      0x7D
#define ACCEL_SOFTRESET     0x7E

#define GYRO_CHIP_ID        0x00
#define GYRO_SOFTRESET      0x14

#define BMI088_SOFTRESET_VALUE  0xB6


static const BMI088_Config *g_cfg;

static uint8_t SPI_RW(uint8_t tx_data)
{
    DL_SPI_transmitData8(g_cfg->spi, tx_data);
    while (DL_SPI_isRXFIFOEmpty(g_cfg->spi));
    uint8_t rx = DL_SPI_receiveData8(g_cfg->spi);
    while (DL_SPI_isBusy(g_cfg->spi));
    return rx;
}

static void CS_Select(uint8_t cs_sel)
{
    if (cs_sel == 0) {
        DL_GPIO_clearPins(g_cfg->csAccelPort, g_cfg->csAccelPin);
    } else {
        DL_GPIO_clearPins(g_cfg->csGyroPort, g_cfg->csGyroPin);
    }
}

static void CS_Deselect(uint8_t cs_sel)
{
    if (cs_sel == 0) {
        DL_GPIO_setPins(g_cfg->csAccelPort, g_cfg->csAccelPin);
    } else {
        DL_GPIO_setPins(g_cfg->csGyroPort, g_cfg->csGyroPin);
    }
}

static void BMI088_WriteReg(uint8_t reg, uint8_t data, uint8_t cs_sel)
{
    CS_Select(cs_sel);

    SPI_RW(reg & 0x7F);
    SPI_RW(data);

    delay_ms(1);

    CS_Deselect(cs_sel);
}

static void BMI088_ReadBytes(uint8_t reg, uint8_t *buf, uint8_t len, uint8_t cs_sel)
{
    CS_Select(cs_sel);

    SPI_RW(reg | 0x80);

    if (cs_sel == 0) {
        SPI_RW(0xFF);
    }

    for (uint8_t i = 0; i < len; i++) {
        buf[i] = SPI_RW(0xFF);
    }

    CS_Deselect(cs_sel);
}

void BMI088_ReadRawBytes(uint8_t reg, uint8_t *buf, uint8_t len, uint8_t cs_sel)
{
    BMI088_ReadBytes(reg, buf, len, cs_sel);
}

uint8_t BMI088_ReadAccelID(void)
{
    uint8_t id;
    BMI088_ReadBytes(ACCEL_CHIP_ID, &id, 1, 0);
    return id;
}

uint8_t BMI088_ReadGyroID(void)
{
    uint8_t id;
    BMI088_ReadBytes(GYRO_CHIP_ID, &id, 1, 1);
    return id;
}

static uint8_t BMI088_Accel_Init(void)
{
    BMI088_WriteReg(ACCEL_SOFTRESET, BMI088_SOFTRESET_VALUE, 0);
    delay_ms(10);

    BMI088_WriteReg(ACCEL_PWR_CTRL, 0x04, 0);
    delay_ms(5);

    return 0;
}

static uint8_t BMI088_Gyro_Init(void)
{
    BMI088_WriteReg(GYRO_SOFTRESET, BMI088_SOFTRESET_VALUE, 1);
    delay_ms(30);

    return 0;
}

uint8_t BMI088_Init(const BMI088_Config *cfg)
{
    g_cfg = cfg;
    uint8_t err = 0;
    err |= BMI088_Accel_Init();
    err |= BMI088_Gyro_Init();
    return err;
}

void BMI088_ReadAccel(float accel[3])
{
    uint8_t buf[6];
    BMI088_ReadBytes(ACCEL_X_LSB, buf, 6, 0);

    int16_t raw_x = (int16_t)((buf[1] << 8) | buf[0]);
    int16_t raw_y = (int16_t)((buf[3] << 8) | buf[2]);
    int16_t raw_z = (int16_t)((buf[5] << 8) | buf[4]);

    accel[0] = (float)raw_x * BMI088_ACCEL_SENSITIVITY;
    accel[1] = (float)raw_y * BMI088_ACCEL_SENSITIVITY;
    accel[2] = (float)raw_z * BMI088_ACCEL_SENSITIVITY;
}

void BMI088_ReadGyro(float gyro[3])
{
    uint8_t buf[8] = {0};
    BMI088_ReadBytes(GYRO_CHIP_ID, buf, 8, 1);

    if (buf[0] == 0x0F) {
        int16_t raw_x = (int16_t)((buf[3] << 8) | buf[2]);
        int16_t raw_y = (int16_t)((buf[5] << 8) | buf[4]);
        int16_t raw_z = (int16_t)((buf[7] << 8) | buf[6]);

        gyro[0] = (float)raw_x * BMI088_GYRO_2000_SEN;
        gyro[1] = (float)raw_y * BMI088_GYRO_2000_SEN;
        gyro[2] = (float)raw_z * BMI088_GYRO_2000_SEN;
    }
}
