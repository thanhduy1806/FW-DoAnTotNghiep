/************************************************
 *  @file     : app_alive.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef app_alive_H
#define app_alive_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os.h"

extern const osThreadAttr_t alive_attr;

void App_AliveTask(void *param);

#ifdef __cplusplus
}
#endif

#endif /* app_alive_H */