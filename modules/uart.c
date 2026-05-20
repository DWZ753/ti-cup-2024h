#include "UART.h"

volatile uint8_t UART0TxDMADone = 1; // UART0发送DMA完成标志
volatile uint8_t UART0RxDone = 0; // UART0接收完成标志
volatile uint8_t UART0RxBuf[UART_RX_BUF_SIZE] = {0}; // UART0接收缓冲区
volatile uint16_t UART0RxPos = 0; // UART0接收位置
volatile uint16_t UART0RxLen = 0; // UART0接收长度(不包含结束符)
volatile uint8_t UART0RxOvf = 0; // UART0接收溢出数据


void UART_init(void) {
    NVIC_ClearPendingIRQ(PRINT_INST_INT_IRQN);
    NVIC_EnableIRQ(PRINT_INST_INT_IRQN);
}

/**
 * @brief UART0发送字符串
 * @note 使用阻塞方式
 * @param str 待发送字符串指针
 * @return 字符串长度
 */
int UART0_sendStr(const char* str) {
    int cnt = 0;
    while (*str) {
        DL_UART_transmitDataBlocking(PRINT_INST, (uint8_t)*str);
        str++;
        cnt++;
    }
    return cnt;
}

/**
 * @brief UART0 printf
 * @param fmt 格式控制字符串与参数列表
 * @return 字符串长度(vsprintf返回值)
 */
int UART0_printf(char* fmt, ...) {
    static char buf[UART_TX_BUF_SIZE];
    int len;
    va_list args;
    va_start(args, fmt);
    len = vsprintf(buf, fmt, args);
    va_end(args);
    UART0_sendStr(buf);
    return len;
}

/**
 * @brief UART0使用DMA方式发送字符串
 * @note 调用该函数时, 若上次UART DMA已传送完成, 则占用时间最短
 * @param str 待发送字符串指针
 * @param len 字符串长度
 */
void UART0_sendStrDMA(const char* str, uint16_t len) {
    while (!UART0TxDMADone);
    UART0TxDMADone = 0;
    DL_DMA_setSrcAddr(DMA, UART0_DMA_TX_CHAN_ID, (uint32_t)str);
    DL_DMA_setDestAddr(DMA, UART0_DMA_TX_CHAN_ID, (uint32_t)(&PRINT_INST->TXDATA));
    DL_DMA_setTransferSize(DMA, UART0_DMA_TX_CHAN_ID, len);
    DL_DMA_enableChannel(DMA, UART0_DMA_TX_CHAN_ID);
}

/**
 * @brief UART0 printf (使用DMA方式)
 * @note 调用该函数时, 若上次UART DMA已传送完成, 则占用时间最短
 * @param fmt 格式控制字符串与参数列表
 */
void UART0_printfDMA(char* fmt, ...) {
    static char buf[UART_TX_BUF_SIZE];
    uint16_t len;
    va_list args;
    while (!UART0TxDMADone);
    va_start(args, fmt);
    len = (uint16_t)vsprintf(buf, fmt, args);
    va_end(args);
    UART0_sendStrDMA(buf, len);
}

/**
 * @brief UART0开始接收数据
 * @note 先处理完上次接收数据, 再调用该函数继续接收
 */
void UART0_startReceive(void) {
    UART0RxPos = 0;
    UART0RxDone = 0;
}

// UART0 DMA Tx完成中断回调
void UART0_DMADoneTxCallback(void) {
    UART0TxDMADone = 1;
}

// UART0 Rx中断回调
void UART0_RxCallback(void) {
    if (!UART0RxDone) { // 上次数据处理完成后, 继续接收

        // 接收当前字节
        UART0RxBuf[UART0RxPos] = DL_UART_receiveData(PRINT_INST);

        // 判断结束符
        if (UART0RxBuf[UART0RxPos] == UART_RX_TERMINATOR) {
            UART0RxBuf[UART0RxPos] = '\0';
            UART0RxLen = UART0RxPos;
            UART0RxDone = 1;
        }
        
        UART0RxPos++;
    }
    else { // 未及时处理数据放入溢出区
        UART0RxOvf = DL_UART_receiveData(PRINT_INST);
    }
}


// UART0中断服务函数
void PRINT_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(PRINT_INST)) {
        case DL_UART_IIDX_DMA_DONE_TX:
            UART0_DMADoneTxCallback();
            break;
        case DL_UART_IIDX_RX:
            UART0_RxCallback();
            break;
        default:
            break;
    }
}