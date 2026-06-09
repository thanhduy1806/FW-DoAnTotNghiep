/************************************************
 *  @file     : app_usb.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef app_usb_H
#define app_usb_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os.h"

extern const osThreadAttr_t usb_attr;

void App_USBTask(void *param);

#ifdef __cplusplus
}
#endif

#endif /* app_usb_H */