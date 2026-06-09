/************************************************
 *  @file     : board.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef board_H
#define board_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================*/
/*                        Includes                            */
/*============================================================*/
#include <stdint.h>
#include "M2_BSP/UART/uart_irq.h"

/*============================================================*/
/*                         Defines                            */
/*============================================================*/
#define BOARD_MCK               120000000UL 


/*============================================================*/
/*                   Public Variables                         */
/*============================================================*/
#define DEBUG_WriteByte(data)    UART_Driver_WriteByte(UART2_REGS, data)
#define DEBUG_Write(data, len)   UART_Driver_Write(UART2_REGS, data, len)
#define DEBUG_SendString(str)    UART_Driver_SendString(UART2_REGS, str)

/*============================================================*/
/*                Function Prototypes                         */
/*============================================================*/

#ifdef __cplusplus
}
#endif

#endif /* board_H */