#ifndef __USER_UART_H__
#define __USER_UART_H__

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "ti_msp_dl_config.h"

/* ========== 硬件映射宏（SysConfig 重新生成后只需修改此部分即可适配） ========== */
#define UART_PRINT_INST             PRINT_INST              /* UART 外设实例         */
#define UART_PRINT_INT_IRQN         PRINT_INST_INT_IRQN      /* UART 中断号           */
#define UART_PRINT_IRQHandler       PRINT_INST_IRQHandler    /* UART 中断服务函数名    */

/* ========== 通用参数宏 ========== */
#define UART_TX_BUF_SIZE            256   /* 发送缓冲区长度（字节） */
#define UART_RX_BUF_SIZE            256   /* 接收缓冲区长度（字节） */
#define UART_RX_TERMINATOR          '\n'  /* 接收结束符             */

/* ========== 类型定义 ========== */

/** UART 初始化配置 */
typedef struct {
    UART_Regs  *uart;          /* UART 外设基址指针（如 UART0） */
    IRQn_Type   irqNum;        /* NVIC 中断号                    */
    uint8_t     dmaTxChanId;   /* DMA TX 通道号                  */
    uint8_t     dmaTxTrigger;  /* DMA 发送触发源                 */
} UART_Config;

/** UART 运行时句柄，封装一个 UART 实例的全部状态 */
typedef struct {
    /* ---- 硬件引用 ---- */
    UART_Regs  *uart;          /* UART 外设基址指针          */
    IRQn_Type   irqNum;        /* NVIC 中断号                */
    DMA_Regs   *dma;           /* DMA 控制器基址指针          */
    uint8_t     dmaTxChanId;   /* DMA TX 通道号              */
    uint8_t     dmaTxTrigger;  /* DMA 发送触发源             */

    /* ---- 发送状态 ---- */
    volatile uint8_t  txDMADone;                        /* DMA 发送完成标志       */

    /* ---- 接收状态 ---- */
    volatile uint8_t  rxDone;                           /* 接收完成标志           */
    volatile uint8_t  rxBuf[UART_RX_BUF_SIZE];          /* 接收缓冲区             */
    volatile uint16_t rxPos;                            /* 当前接收位置           */
    volatile uint16_t rxLen;                            /* 接收长度（不含结束符）  */
    volatile uint8_t  rxOvf;                            /* 溢出数据               */
} UART_Handle;

/* ========== 通用 API ========== */

UART_Handle* UART_Init(const UART_Config *config);
int  UART_SendStr(UART_Handle *h, const char *str);
int  UART_Printf(UART_Handle *h, char *fmt, ...);
void UART_SendStrDMA(UART_Handle *h, const char *str, uint16_t len);
void UART_PrintfDMA(UART_Handle *h, char *fmt, ...);
void UART_StartReceive(UART_Handle *h);
void UART_HandleIRQ(UART_Handle *h);

/* ========== ISR 入口（由向量表调用，自动分发到已注册句柄） ========== */

void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);
void UART3_IRQHandler(void);

#endif
