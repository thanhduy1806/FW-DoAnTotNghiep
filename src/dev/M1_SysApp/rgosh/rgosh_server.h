/************************************************
 *  @file     : rgosh_server.h
 *  @date     : February 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description:
 ************************************************/

#ifndef APP_RGOSH_H
#define APP_RGOSH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <csp/csp.h>
#include "rgosh_error.h"

#define RGOSH_OUT_BUF_SIZE  512

#ifndef GS_CSP_PORT_RGOSH
#define GS_CSP_PORT_RGOSH           12
#endif

xerr_t rgosh_init(void);
void rgosh_handle_conn(csp_conn_t *conn);

#ifdef __cplusplus
}
#endif

#endif /* APP_RGOSH_H */