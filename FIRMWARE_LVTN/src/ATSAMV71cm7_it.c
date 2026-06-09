#include "interrupts.h"
#include "peripheral/systick/plib_systick.h"
#include "peripheral/pio/plib_pio.h"
#include "peripheral/tc/plib_tc3.h"
#include "M5_Utils/DateTime/date_time.h"
#include "M5_Utils/Tick/tick.h"
#include "stdio.h"
#include "tusb.h"

/******************************************************************************/
/*            Cortex Processor Interruption and Exception Handlers            */
/******************************************************************************/

/*****************************************************************************/
/*                              Sys-tick Handler                             */
/*****************************************************************************/
static void EX_Systick_Handler(uintptr_t context)
{
    (void)context;

    
}

void EX_SysTick_HandlerRegister(void)
{
    SYSTICK_TimerCallbackSet(EX_Systick_Handler, 0);
}

/*****************************************************************************/
/*                                Timer Handler                              */
/*****************************************************************************/
static uint32_t tick = 0;
static uint8_t toggleState = false;
static void EX_TC3_CH0_Handler(TC_TIMER_STATUS status, uintptr_t context)
{
    (void)context;
    (void)status;

    tick++;
    if (tick >= 1000)
    {
        tick = 0;
        if (toggleState)
        {
            
            LED_PC9_Clear();
        }
        else
        {
            LED_PC9_Set();
        }
        toggleState = 1 - toggleState;

        Utils_SoftTime_Update(); // FREERTOS
    }

    TickTimer_IRQHandler(); 

}
void EX_TC3_CH0_HandlerRegister(void)
{
    TC3_CH0_TimerCallbackRegister(EX_TC3_CH0_Handler, 0);
}

/*****************************************************************************/
/*                                USART1 Handler                             */
/*****************************************************************************/


/******************************************************************************/
/*                                  USBHS Handler                             */
/******************************************************************************/
void __attribute__((used)) USBHS_Handler( void )
{
    tud_int_handler(0);
}