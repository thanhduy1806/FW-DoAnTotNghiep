/*
 * xcli_log.h  -  xCLI debug logging.
 *                                                  C.H
 */
#ifndef XCLI_LOG_H
#define XCLI_LOG_H

#include "xcli_config.h"
#include "M1_SysApp/xtp/xtp_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#if XCLI_ENABLE_LOG
#  define xCLI_Log(fmt, ...)  xTP_Log("[xCLI] " fmt, ##__VA_ARGS__)
#else
#  define xCLI_Log(...)       ((void)0)
#endif

#ifdef __cplusplus
}
#endif
#endif /* XCLI_LOG_H */
