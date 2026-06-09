/************************************************
 *  @file     : ATSAMV71cm7_it.h
 *  @date     : November 2025
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef ATSAMV71cm7_it_H
#define ATSAMV71cm7_it_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================*/
/*                        Includes                            */
/*============================================================*/
#include <stdint.h>

void EX_SysTick_HandlerRegister(void);
void EX_TC3_CH0_HandlerRegister(void);

#ifdef __cplusplus
}
#endif

#endif /* ATSAMV71cm7_it_H */