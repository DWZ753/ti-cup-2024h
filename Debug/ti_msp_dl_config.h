/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_MOTOR */
#define PWM_MOTOR_INST                                                     TIMA0
#define PWM_MOTOR_INST_IRQHandler                               TIMA0_IRQHandler
#define PWM_MOTOR_INST_INT_IRQN                                 (TIMA0_INT_IRQn)
#define PWM_MOTOR_INST_CLK_FREQ                                         32000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_MOTOR_C0_PORT                                             GPIOB
#define GPIO_PWM_MOTOR_C0_PIN                                     DL_GPIO_PIN_14
#define GPIO_PWM_MOTOR_C0_IOMUX                                  (IOMUX_PINCM31)
#define GPIO_PWM_MOTOR_C0_IOMUX_FUNC                 IOMUX_PINCM31_PF_TIMA0_CCP0
#define GPIO_PWM_MOTOR_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_C1_PORT                                             GPIOA
#define GPIO_PWM_MOTOR_C1_PIN                                     DL_GPIO_PIN_22
#define GPIO_PWM_MOTOR_C1_IOMUX                                  (IOMUX_PINCM47)
#define GPIO_PWM_MOTOR_C1_IOMUX_FUNC                 IOMUX_PINCM47_PF_TIMA0_CCP1
#define GPIO_PWM_MOTOR_C1_IDX                                DL_TIMER_CC_1_INDEX

/* Defines for PWM_SERVO */
#define PWM_SERVO_INST                                                     TIMA1
#define PWM_SERVO_INST_IRQHandler                               TIMA1_IRQHandler
#define PWM_SERVO_INST_INT_IRQN                                 (TIMA1_INT_IRQn)
#define PWM_SERVO_INST_CLK_FREQ                                          1000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_SERVO_C0_PORT                                             GPIOB
#define GPIO_PWM_SERVO_C0_PIN                                      DL_GPIO_PIN_2
#define GPIO_PWM_SERVO_C0_IOMUX                                  (IOMUX_PINCM15)
#define GPIO_PWM_SERVO_C0_IOMUX_FUNC                 IOMUX_PINCM15_PF_TIMA1_CCP0
#define GPIO_PWM_SERVO_C0_IDX                                DL_TIMER_CC_0_INDEX



/* Defines for PIT_FOR_CUSTOM */
#define PIT_FOR_CUSTOM_INST                                             (TIMG12)
#define PIT_FOR_CUSTOM_INST_IRQHandler                         TIMG12_IRQHandler
#define PIT_FOR_CUSTOM_INST_INT_IRQN                           (TIMG12_INT_IRQn)
#define PIT_FOR_CUSTOM_INST_LOAD_VALUE                                   (3999U)
/* Defines for PIT_FOR_CONTROL */
#define PIT_FOR_CONTROL_INST                                             (TIMG0)
#define PIT_FOR_CONTROL_INST_IRQHandler                         TIMG0_IRQHandler
#define PIT_FOR_CONTROL_INST_INT_IRQN                           (TIMG0_INT_IRQn)
#define PIT_FOR_CONTROL_INST_LOAD_VALUE                                 (15999U)




/* Port definition for Pin Group GPIO_BUZZERs */
#define GPIO_BUZZERs_PORT                                                (GPIOB)

/* Defines for GPIO_BUZZER: GPIOB.5 with pinCMx 18 on package pin 53 */
#define GPIO_BUZZERs_GPIO_BUZZER_PIN                             (DL_GPIO_PIN_5)
#define GPIO_BUZZERs_GPIO_BUZZER_IOMUX                           (IOMUX_PINCM18)
/* Port definition for Pin Group GPIO_LEDs */
#define GPIO_LEDs_PORT                                                   (GPIOB)

/* Defines for GPIO_LED: GPIOB.22 with pinCMx 50 on package pin 21 */
#define GPIO_LEDs_GPIO_LED_PIN                                  (DL_GPIO_PIN_22)
#define GPIO_LEDs_GPIO_LED_IOMUX                                 (IOMUX_PINCM50)
/* Port definition for Pin Group GPIO_MOTORs */
#define GPIO_MOTORs_PORT                                                 (GPIOB)

/* Defines for GPIO_MOTOR1_IN1: GPIOB.0 with pinCMx 12 on package pin 47 */
#define GPIO_MOTORs_GPIO_MOTOR1_IN1_PIN                          (DL_GPIO_PIN_0)
#define GPIO_MOTORs_GPIO_MOTOR1_IN1_IOMUX                        (IOMUX_PINCM12)
/* Defines for GPIO_MOTOR1_IN2: GPIOB.21 with pinCMx 49 on package pin 20 */
#define GPIO_MOTORs_GPIO_MOTOR1_IN2_PIN                         (DL_GPIO_PIN_21)
#define GPIO_MOTORs_GPIO_MOTOR1_IN2_IOMUX                        (IOMUX_PINCM49)
/* Defines for GPIO_MOTOR2_IN1: GPIOB.8 with pinCMx 25 on package pin 60 */
#define GPIO_MOTORs_GPIO_MOTOR2_IN1_PIN                          (DL_GPIO_PIN_8)
#define GPIO_MOTORs_GPIO_MOTOR2_IN1_IOMUX                        (IOMUX_PINCM25)
/* Defines for GPIO_MOTOR2_IN2: GPIOB.24 with pinCMx 52 on package pin 23 */
#define GPIO_MOTORs_GPIO_MOTOR2_IN2_PIN                         (DL_GPIO_PIN_24)
#define GPIO_MOTORs_GPIO_MOTOR2_IN2_IOMUX                        (IOMUX_PINCM52)
/* Port definition for Pin Group GPIO_KEYs */
#define GPIO_KEYs_PORT                                                   (GPIOB)

/* Defines for KEY1: GPIOB.18 with pinCMx 44 on package pin 15 */
#define GPIO_KEYs_KEY1_PIN                                      (DL_GPIO_PIN_18)
#define GPIO_KEYs_KEY1_IOMUX                                     (IOMUX_PINCM44)
/* Defines for KEY2: GPIOB.23 with pinCMx 51 on package pin 22 */
#define GPIO_KEYs_KEY2_PIN                                      (DL_GPIO_PIN_23)
#define GPIO_KEYs_KEY2_IOMUX                                     (IOMUX_PINCM51)
/* Defines for KEY3: GPIOB.19 with pinCMx 45 on package pin 16 */
#define GPIO_KEYs_KEY3_PIN                                      (DL_GPIO_PIN_19)
#define GPIO_KEYs_KEY3_IOMUX                                     (IOMUX_PINCM45)
/* Defines for KEY4: GPIOB.10 with pinCMx 27 on package pin 62 */
#define GPIO_KEYs_KEY4_PIN                                      (DL_GPIO_PIN_10)
#define GPIO_KEYs_KEY4_IOMUX                                     (IOMUX_PINCM27)
#define GPIOB_EVENT_SUBSCRIBER_0_CHANNEL                                     (1)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_MOTOR_init(void);
void SYSCFG_DL_PWM_SERVO_init(void);
void SYSCFG_DL_PIT_FOR_CUSTOM_init(void);
void SYSCFG_DL_PIT_FOR_CONTROL_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
