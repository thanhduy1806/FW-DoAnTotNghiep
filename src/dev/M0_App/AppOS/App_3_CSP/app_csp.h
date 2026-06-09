/************************************************
 *  @file     : app_csp.h
 *  @date     : February 2026
 *  @author   : CAO HIEU
 *  @version  : 3.0.0
 ************************************************/

#ifndef APP_CSP_H
#define APP_CSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os.h"
#include <stdbool.h>
#include <stdint.h>

/* ---- CSP node configuration ---- */
#define CSP_MY_ADDRESS   12
#define CSP_CAN_PROMISC  true

/* ---- Task ---- */
extern const osThreadAttr_t csp_attr;
void App_CSPTask(void *param);

/* ---- CLI helper ---- */
bool CAN_RawSend(uint32_t canId, uint8_t *data, uint8_t dlc, bool extended);

#ifdef __cplusplus
}
#endif

#endif /* APP_CSP_H */