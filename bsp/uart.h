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

/**
 * @brief 注册并初始化一个 UART 实例
 * @param config 初始化配置（指定外设、中断号、DMA 通道等）
 * @return 成功返回句柄指针，失败返回 NULL（重复注册 / 硬件索引无效 / 实例已满）
 * @note 调用后 NVIC 中断自动使能；DMA 初始化由 SYSCFG_DL_init() 完成
 */
UART_Handle* UART_Init(const UART_Config *config);

/**
 * @brief 阻塞方式发送字符串（逐字节等待）
 * @param h   UART 句柄指针
 * @param str 待发送字符串（以 \0 结尾）
 * @return 已发送的字符数
 */
int  UART_SendStr(UART_Handle *h, const char *str);

/**
 * @brief 阻塞方式格式化输出（printf 风格）
 * @param h   UART 句柄指针
 * @param fmt 格式控制字符串
 * @param ... 可变参数列表
 * @return 格式化后的字符串长度
 */
int  UART_Printf(UART_Handle *h, char *fmt, ...);

/**
 * @brief DMA 方式发送字符串（非阻塞）
 * @param h   UART 句柄指针
 * @param str 待发送字符串
 * @param len 字符串长度
 * @note 调用后需等待 h->txDMADone == 1；发送期间会等待上一次 DMA 传输完成
 */
void UART_SendStrDMA(UART_Handle *h, const char *str, uint16_t len);

/**
 * @brief DMA 方式格式化输出（非阻塞）
 * @param h   UART 句柄指针
 * @param fmt 格式控制字符串
 * @param ... 可变参数列表
 * @note 调用后需等待 h->txDMADone == 1；发送期间会等待上一次 DMA 传输完成
 */
void UART_PrintfDMA(UART_Handle *h, char *fmt, ...);

/**
 * @brief 开始接收数据，重置接收状态
 * @param h UART 句柄指针
 * @note 调用后 rxDone 清零、rxPos 归零，ISR 收到数据后自动填充缓冲区
 */
void UART_StartReceive(UART_Handle *h);

/**
 * @brief 通用 UART 中断处理（由各 UARTx_IRQHandler 分发调用）
 * @param h UART 句柄指针
 * @note 处理 DMA 发送完成和 RX 接收两种中断
 */
void UART_HandleIRQ(UART_Handle *h);

/* ========== ISR 入口（由向量表调用，自动分发到已注册句柄） ========== */

void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);
void UART3_IRQHandler(void);

#endif
