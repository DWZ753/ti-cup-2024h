#include "UART.h"

#define UART_MAX_INSTANCES 4

/* ========== 句柄注册表 ========== */
static UART_Handle g_handle_pool[UART_MAX_INSTANCES];
static UART_Handle *g_uart_instances[UART_MAX_INSTANCES];
static uint8_t g_uart_count;

/**
 * @brief 根据 UART 外设基址确定硬件索引
 * @param uart UART 外设基址指针
 * @return 硬件索引 0~3，未匹配返回 UART_MAX_INSTANCES
 */
static uint8_t get_uart_index(UART_Regs *uart)
{
    if (uart == UART0) return 0;
    if (uart == UART1) return 1;
    if (uart == UART2) return 2;
    if (uart == UART3) return 3;
    return UART_MAX_INSTANCES;
}

/* ========== 通用 API ========== */

/**
 * @brief 注册并初始化一个 UART 实例
 * @param config 初始化配置（指定外设、中断号、DMA 通道等）
 * @return 成功返回句柄指针，失败返回 NULL（重复注册 / 硬件索引无效 / 实例已满）
 * @note 调用后 NVIC 中断自动使能，DMA 初始化由 SYSCFG_DL_init() 完成
 */
UART_Handle* UART_Init(const UART_Config *config)
{
    uint8_t idx;
    uint16_t i;

    if (config == NULL || config->uart == NULL) return NULL;
    idx = get_uart_index(config->uart);
    if (idx >= UART_MAX_INSTANCES) return NULL;
    if (g_uart_instances[idx] != NULL) return NULL;
    if (g_uart_count >= UART_MAX_INSTANCES) return NULL;

    UART_Handle *h = &g_handle_pool[g_uart_count];

    h->uart         = config->uart;
    h->irqNum       = config->irqNum;
    h->dma          = DMA;
    h->dmaTxChanId  = config->dmaTxChanId;
    h->dmaTxTrigger = config->dmaTxTrigger;

    h->txDMADone = 1;
    h->rxDone    = 0;
    h->rxPos     = 0;
    h->rxLen     = 0;
    h->rxOvf     = 0;
    for (i = 0; i < UART_RX_BUF_SIZE; i++) {
        h->rxBuf[i] = 0;
    }

    g_uart_instances[idx] = h;
    g_uart_count++;

    NVIC_ClearPendingIRQ(config->irqNum);
    NVIC_EnableIRQ(config->irqNum);

    return h;
}

/**
 * @brief 阻塞方式发送字符串
 * @param h  UART 句柄指针
 * @param str 待发送字符串（以 '\0' 结尾）
 * @return 已发送的字符数
 */
int UART_SendStr(UART_Handle *h, const char *str)
{
    int cnt = 0;
    while (*str) {
        DL_UART_transmitDataBlocking(h->uart, (uint8_t)*str);
        str++;
        cnt++;
    }
    return cnt;
}

/**
 * @brief 阻塞方式格式化输出（printf 风格）
 * @param h   UART 句柄指针
 * @param fmt 格式控制字符串
 * @param ... 可变参数列表
 * @return 格式化后的字符串长度
 */
int UART_Printf(UART_Handle *h, char *fmt, ...)
{
    static char buf[UART_TX_BUF_SIZE];
    int len;
    va_list args;
    va_start(args, fmt);
    len = vsprintf(buf, fmt, args);
    va_end(args);
    UART_SendStr(h, buf);
    return len;
}

/**
 * @brief DMA 方式发送字符串（非阻塞，需等待 txDMADone 标志）
 * @param h   UART 句柄指针
 * @param str 待发送字符串
 * @param len 字符串长度
 * @note 发送期间会等待上一次 DMA 传输完成（忙等 txDMADone）
 */
void UART_SendStrDMA(UART_Handle *h, const char *str, uint16_t len)
{
    while (!h->txDMADone);
    h->txDMADone = 0;
    DL_DMA_setSrcAddr(h->dma, h->dmaTxChanId, (uint32_t)str);
    DL_DMA_setDestAddr(h->dma, h->dmaTxChanId, (uint32_t)(&h->uart->TXDATA));
    DL_DMA_setTransferSize(h->dma, h->dmaTxChanId, len);
    DL_DMA_enableChannel(h->dma, h->dmaTxChanId);
}

/**
 * @brief DMA 方式格式化输出（非阻塞，需等待 txDMADone 标志）
 * @param h   UART 句柄指针
 * @param fmt 格式控制字符串
 * @param ... 可变参数列表
 * @note 发送期间会等待上一次 DMA 传输完成（忙等 txDMADone）
 */
void UART_PrintfDMA(UART_Handle *h, char *fmt, ...)
{
    static char buf[UART_TX_BUF_SIZE];
    uint16_t len;
    va_list args;
    while (!h->txDMADone);
    va_start(args, fmt);
    len = (uint16_t)vsprintf(buf, fmt, args);
    va_end(args);
    UART_SendStrDMA(h, buf, len);
}

/**
 * @brief 开始接收数据（重置接收状态）
 * @param h UART 句柄指针
 * @note 调用后 rxDone 清零，rxPos 归零，等待中断接收数据
 */
void UART_StartReceive(UART_Handle *h)
{
    h->rxPos  = 0;
    h->rxDone = 0;
}

/* ========== 通用中断处理 ========== */

/**
 * @brief 通用 UART 中断处理（由各 UARTx_IRQHandler 调用）
 * @param h UART 句柄指针
 * @note 处理 DMA 发送完成和接收数据两种中断
 */
void UART_HandleIRQ(UART_Handle *h)
{
    switch (DL_UART_getPendingInterrupt(h->uart)) {
        case DL_UART_IIDX_DMA_DONE_TX:
            h->txDMADone = 1;
            break;
        case DL_UART_IIDX_RX:
            if (!h->rxDone) {
                h->rxBuf[h->rxPos] = DL_UART_receiveData(h->uart);
                if (h->rxBuf[h->rxPos] == UART_RX_TERMINATOR) {
                    h->rxBuf[h->rxPos] = '\0';
                    h->rxLen = h->rxPos;
                    h->rxDone = 1;
                }
                h->rxPos++;
            } else {
                h->rxOvf = DL_UART_receiveData(h->uart);
            }
            break;
        default:
            break;
    }
}

/* ========== ISR 入口 ========== */

void UART0_IRQHandler(void)
{
    if (g_uart_instances[0] != NULL) {
        UART_HandleIRQ(g_uart_instances[0]);
    }
}

void UART1_IRQHandler(void)
{
    if (g_uart_instances[1] != NULL) {
        UART_HandleIRQ(g_uart_instances[1]);
    }
}

void UART2_IRQHandler(void)
{
    if (g_uart_instances[2] != NULL) {
        UART_HandleIRQ(g_uart_instances[2]);
    }
}

void UART3_IRQHandler(void)
{
    if (g_uart_instances[3] != NULL) {
        UART_HandleIRQ(g_uart_instances[3]);
    }
}
