/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
#include "empty.h"

static UART_Handle *uart_print;

int state = 0;

void test(void);

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

    while (1)
    {
        test();

        // Buzzer_Beep(1000);
        // delay_ms(2000);
    }
}


void test(void)
{
    static uint8_t t = ' ';

    OLED_ShowChinese(0,0,0,16);//中
    OLED_ShowChinese(18,0,1,16);//景
    OLED_ShowChinese(36,0,2,16);//园
    OLED_ShowChinese(54,0,3,16);//电
    OLED_ShowChinese(72,0,4,16);//子
    OLED_ShowChinese(90,0,5,16);//科
    OLED_ShowChinese(108,0,6,16);//技
    OLED_ShowString(8,2,(uint8_t *)"ZHONGJINGYUAN",16);
    OLED_ShowString(20,4,(uint8_t *)"2014/05/01",16);
    OLED_ShowString(0,6,(uint8_t *)"ASCII:",16);  
    OLED_ShowString(63,6,(uint8_t *)"CODE:",16);
    OLED_ShowChar(48,6,t,16);
    t++;
    if(t>'~')t=' ';
    OLED_ShowNum(103,6,t,3,16);
    delay_ms(500);
    OLED_Clear();

}
