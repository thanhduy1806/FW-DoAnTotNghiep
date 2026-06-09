/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "stdio.h"
#include "string.h"

// *****************************************************************************
#include "M0_App/Boot/boot_manager.h"

#include "M3_Driver/devices/MB85RS2MT/mb85rs2mt.h"
#include "M3_Driver/devices/IS66WVS8M8/is66wvs8m8.h"
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
//void USART2_DMA_Callback(XDMAC_TRANSFER_EVENT event, uintptr_t context)
//{
//    if (event == XDMAC_TRANSFER_COMPLETE)
//    {
//        while (!(USART2_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk));
//        PIOD_REGS->PIO_SODR = ((uint32_t)1U<<19U);
//    }
//}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    XDMAC_ChannelCallbackRegister(XDMAC_CHANNEL_0, SPI1_DMA_TX_Callback, 0);
    XDMAC_ChannelCallbackRegister(XDMAC_CHANNEL_1, SPI1_DMA_RX_Callback, 0);
    
    BootManager_SystemInit();
    
    BootManager_SystemStart();
    
    for(;;)
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

