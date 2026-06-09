/************************************************
 *  @file     : app_cli.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef app_cli_H
#define app_cli_H

#ifdef __cplusplus
extern "C" {
#endif
#include "os.h"

void app_cli_wizard_start(uint8_t total_step);

extern const osThreadAttr_t cli_attr;

void App_CLITask(void *param);

#ifdef __cplusplus
}
#endif

#endif /* app_cli_H */