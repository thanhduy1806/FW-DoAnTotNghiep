/************************************************
 *  @file     : app_logging.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef app_logging_H
#define app_logging_H

#include "os.h"

extern const osThreadAttr_t log_attr;

void App_LogTask(void *param);


#endif /* app_logging_H */