/*
 * xtp_port.c
 * Replace each stub with your actual driver calls.
 *                                                  C.H
 */

#include "xtp_port.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* --- Include your platform headers below -------------------------------- */
#include "M2_BSP/UART/uart_irq.h"
#include "M5_Utils/Tick/tick.h"
/* ----------------------------------------------------------------------- */

void *xTP_MemSet(void *ptr, int value, size_t num)
{
    return memset(ptr, value, num);
}

void xTP_SendByte(const uint8_t *byte)
{
    if (byte == NULL) { return; }
    UART0_WriteByte(*byte);
}

int xTP_ReadByte(uint8_t *byte)
{
    if (byte == NULL) { return -1; }
    int d = UART0_ReadByte();
    if (d < 0) { return -1; }
    *byte = (uint8_t)d;
    return 0;
}

uint16_t xTP_GetSpaceForTx(void)
{
    /* TODO: return UART_Driver_TXNumFreeSlots(USART6); */
    return 0xFFFFU;
}

uint32_t xTP_GetTick(void)
{
    return Utils_GetTick();
}

xTP_Port_t xTP_GetDefaultPort(void)
{
    xTP_Port_t p;
    p.send_byte    = xTP_SendByte;
    p.read_byte    = xTP_ReadByte;
    p.get_tx_space = xTP_GetSpaceForTx;
    p.get_tick     = xTP_GetTick;
#if XTP_USE_TX_LOCK
    p.tx_lock   = xTP_Port_TxLock;
    p.tx_unlock = xTP_Port_TxUnlock;
#endif
#if XTP_USE_RX_LOCK
    p.rx_lock   = xTP_Port_RxLock;
    p.rx_unlock = xTP_Port_RxUnlock;
#endif
    return p;
}

#if XTP_USE_TX_LOCK
static volatile uint8_t xtp_tx_lock_flag = 0U;
int xTP_Port_TxLock(void)
{
    if (xtp_tx_lock_flag != 0U) { return 1; }
    xtp_tx_lock_flag = 1U;
    return 0;
}
int xTP_Port_TxUnlock(void) { xtp_tx_lock_flag = 0U; return 0; }
#endif

#if XTP_USE_RX_LOCK
static volatile uint8_t xtp_rx_lock_flag = 0U;
int xTP_Port_RxLock(void)
{
    if (xtp_rx_lock_flag != 0U) { return 1; }
    xtp_rx_lock_flag = 1U;
    return 0;
}
int xTP_Port_RxUnlock(void) { xtp_rx_lock_flag = 0U; return 0; }
#endif

/* =========================================================================
 * Logging
 * =========================================================================*/
#if XTP_ENABLE_LOG

void xTP_Log(const char *fmt, ...) { 
    char buf[XTP_LOG_BUFFER_SIZE]; 
    va_list args; 
    int len; 
    va_start(args, fmt); 
    len = vsnprintf(buf, sizeof(buf), fmt, args); 
    va_end(args); 
    UART2_SendString("[x] ");
    UART2_SendString(buf);
    UART2_SendString("\r\n");
}

#endif /* XTP_ENABLE_LOG */
