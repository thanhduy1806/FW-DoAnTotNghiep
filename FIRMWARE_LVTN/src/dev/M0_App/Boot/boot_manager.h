/************************************************
 *  @file     : boot_manager.h
 *  @date     : October 2025
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef boot_manager_H
#define boot_manager_H

#ifdef __cplusplus
extern "C" {
#endif

#include "M5_Utils/Define/define.h"
    
Std_ReturnType BootManager_SystemInit(void);
void BootManager_SystemStart(void);

#ifdef __cplusplus
}
#endif

#endif /* boot_manager_H */