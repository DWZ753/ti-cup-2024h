#ifndef __USER_UART_H__
#define __USER_UART_H__

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "ti_msp_dl_config.h"

#define UART_TX_BUF_SIZE 256 // UART发送缓冲区长度
#define UART_RX_BUF_SIZE   256  // UART接收缓冲区长度
#define UART_RX_TERMINATOR '\n' // UART接收结束符

extern volatile uint8_t UART0TxDMADone;
extern volatile uint8_t UART0RxDone;
extern volatile uint8_t UART0RxBuf[UART_RX_BUF_SIZE];
extern volatile uint16_t UART0RxPos;
extern volatile uint16_t UART0RxLen;
extern volatile uint8_t UART0RxOvf;

void UART_init(void);
int UART0_sendStr(const char* str);
int UART0_printf(char* fmt, ...);
void UART0_sendStrDMA(const char* str, uint16_t len);
void UART0_printfDMA(char* fmt, ...);

void UART0_startReceive(void);
void UART0_DMADoneTxCallback(void);
void UART0_RxCallback(void);

void PRINT_INST_IRQHandler(void);

#endif

