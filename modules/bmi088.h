#ifndef BMI088_H
#define BMI088_H

#include <stdint.h>
#include "ti_msp_dl_config.h"

/* ========== 量程选择（暂未使用，保留以备后续配置） ========== */
#define BMI088_ACCEL_RANGE_3G   0x00
#define BMI088_ACCEL_RANGE_6G   0x01
#define BMI088_ACCEL_RANGE_12G  0x02
#define BMI088_ACCEL_RANGE_24G  0x03

#define BMI088_GYRO_RANGE_2000  0x00

/* ========== 灵敏度系数 ========== */
/* 加速度计: ±6g → m/s², (6*9.8)/32768 ≈ 0.001795 */
#define BMI088_ACCEL_SENSITIVITY  0.001795f
/* 陀螺仪: ±2000°/s → rad/s, (2000*π/180)/32768 ≈ 0.001065 */
#define BMI088_GYRO_2000_SEN      0.00106526443603169529841533860381f

/* ========== 初始化配置 ========== */
typedef struct {
    SPI_Regs   *spi;            /* SPI 外设基址 (如 SPI0)              */
    GPIO_Regs  *csAccelPort;    /* 加速度计 CS 端口                   */
    uint32_t    csAccelPin;     /* 加速度计 CS 引脚位掩码             */
    GPIO_Regs  *csGyroPort;     /* 陀螺仪 CS 端口                     */
    uint32_t    csGyroPin;      /* 陀螺仪 CS 引脚位掩码               */
} BMI088_Config;

/* ========== API ========== */
uint8_t BMI088_Init(const BMI088_Config *cfg);
void    BMI088_ReadAccel(float accel[3]);   /* 输出: m/s² */
void    BMI088_ReadGyro(float gyro[3]);     /* 输出: rad/s */
uint8_t BMI088_ReadAccelID(void);           /* 读加速度计 ID（应为 0x1E） */
uint8_t BMI088_ReadGyroID(void);            /* 读陀螺仪 ID（应为 0x0F） */
void    BMI088_ReadRawBytes(uint8_t reg, uint8_t *buf,
                            uint8_t len, uint8_t cs_sel);  /* 调试：裸读寄存器 */

#endif
