/************************************************
 *  @file     : app_xtp.h
 *  @date     : April 2026
 *  @author   : CAO HIEU
 *  @version  : 2.0.0
 ************************************************/

#ifndef APP_XTP_H
#define APP_XTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os.h"
#include <stdint.h>

#include "M1_SysApp/xtp/xtp.h"
#include "M1_SysApp/xcli/xcli.h"

extern xTP_Instance_t  sam_xtp;
extern xCLI_Instance_t sam_xcli;

/* ---- Task ---- */
extern const osThreadAttr_t xtp_attr;
void App_XTPTask(void *param);

#ifdef __cplusplus
}
#endif

#endif /* APP_XTP_H */