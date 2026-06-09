/************************************************
 *  @file     : boot_manager.c
 *  @date     : November 2025
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "boot_manager.h"
#include "peripheral/systick/plib_systick.h"
#include "peripheral/tc/plib_tc3.h"
#include "board.h"
#include "ATSAMV71cm7_it.h"
#include "hwcsp_cfg.h"

#include "M0_App/AppOS/App_x_Root/app_root.h"
#include "M0_App/AppOS/App_Experiment/app_experiment.h"
#include "M0_App/AppOS/App_4_NTC/app_ntc.h"
#include "M0_App/AppOS/App_6_Temperature_Control/app_temperature.h"

#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"

#include "M2_BSP/UART/uart_irq.h"
#include "M2_BSP/BSP_Solenoid/bsp_solenoid.h"
#include "M2_BSP/BSP_Laser/bsp_laser.h"
#include "M2_BSP/BSP_Photo/bsp_photo.h"
#include "M2_BSP/BSP_FRAM/bsp_fram.h"
#include "M2_BSP/BSP_Flow_Sen/bsp_flow_sen.h"
#include "M2_BSP/BSP_Pump/bsp_pump.h"
#include "M2_BSP/BSP_Watchdog/bsp_watchdog.h"


#include "M3_Devices/FRAM_USART-SPI/fram_usart.h"
#include "M3_Driver/components/i2c/i2c_io.h"

#include "M5_Utils/DateTime/date_time.h"
#include "M2_BSP/BSP_RTC/bsp_rtc.h"


/*============================================================*/
/*                      Defines                               */
/*============================================================*/

/*============================================================*/
/*                   Private Variables                        */
/*============================================================*/

/*============================================================*/
/*                   Public Variables                         */
/*============================================================*/

/*============================================================*/
/*              Private Function Prototypes                   */
/*============================================================*/

/*============================================================*/
/*                Function Implementations                    */
/*============================================================*/
Std_ReturnType BootManager_SystemInit(void){

    Utils_SoftTime_Init();
    EX_SysTick_HandlerRegister();
    EX_TC3_CH0_HandlerRegister();
    
    UART_Driver_Init();
    FRAM_USART_Init(FRAM_MODE_POLLING);
    
    DEBUG_SendString("\r\n");
    DEBUG_SendString("========================================\r\n");
    DEBUG_SendString("          SAM-EXP Booting up....\r\n");
    DEBUG_SendString("========================================\r\n");

    SystemCLI_Init();
    HardwareCSP_Init();
    
    bsp_laser_init();
    bsp_photo_init();
    bsp_watchdog_init();
    ntc_init();
    temperature_init();
    experiment_init();

    return ERROR_OK;
}

void BootManager_SystemStart(void){
    TC3_CH0_TimerStart();
    TC0_CH0_CompareStart();
    TC0_CH2_CompareStart();
    TC1_CH0_CompareStart();
    TC2_CH0_CompareStart();
    TC2_CH1_CompareStart();
    
    SYSTICK_TimerStart();
    {
        static volatile int i;
        static volatile int m = 100;
        while (m--) {
            for (i = 0; i < 10; i++);  
        }
    }
    App_Start();
}