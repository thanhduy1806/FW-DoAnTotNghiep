/*
 * xtp_port.h
 *
 *   tx_lock/tx_unlock  - protects TX path (xTP_SendEx, xTP_ARQ_Send)
 *   rx_lock/rx_unlock  - protects RX path (xTP_Process, FSM state)
 *
 *   Separate locks let TX task and RX task run in parallel without deadlock.
 *   The deferred-ctrl fix ensures no TX call is ever made from the RX lock.
 *
 *   Bare-metal: XTP_USE_TX_LOCK=0, XTP_USE_RX_LOCK=0  (no overhead).
 *   FreeRTOS:   Create two xSemaphoreCreateMutex(), wrap in these callbacks.
 *   Zephyr:     Two struct k_mutex.
 *   POSIX:      Two pthread_mutex_t.
 *                                                  C.H
 */

#ifndef XTP_PORT_H
#define XTP_PORT_H

#include <stdint.h>
#include <stddef.h>
#include "xtp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void     (*send_byte)(const uint8_t *byte);
    int      (*read_byte)(uint8_t *byte);
    uint16_t (*get_tx_space)(void);
    uint32_t (*get_tick)(void);
#if XTP_USE_TX_LOCK
    int      (*tx_lock)(void);
    int      (*tx_unlock)(void);
#endif
#if XTP_USE_RX_LOCK
    int      (*rx_lock)(void);
    int      (*rx_unlock)(void);
#endif
} xTP_Port_t;

void    *xTP_MemSet(void *ptr, int value, size_t num);
void     xTP_SendByte(const uint8_t *byte);
int      xTP_ReadByte(uint8_t *byte);
uint16_t xTP_GetSpaceForTx(void);
uint32_t xTP_GetTick(void);
xTP_Port_t xTP_GetDefaultPort(void);

#if XTP_USE_TX_LOCK
int xTP_Port_TxLock(void);
int xTP_Port_TxUnlock(void);
#endif
#if XTP_USE_RX_LOCK
int xTP_Port_RxLock(void);
int xTP_Port_RxUnlock(void);
#endif

#if XTP_ENABLE_LOG
void xTP_Log(const char *fmt, ...);
#else
#define xTP_Log(...)  ((void)0)
#endif

#ifdef __cplusplus
}
#endif
#endif /* XTP_PORT_H */