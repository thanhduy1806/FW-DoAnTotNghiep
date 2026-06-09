/************************************************
 *  @file     : app_network.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef app_network_H
#define app_network_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os.h"

extern const osThreadAttr_t network_attr;

void App_NetworkTask(void *param);

#ifdef __cplusplus
}
#endif

#endif /* app_network_H */