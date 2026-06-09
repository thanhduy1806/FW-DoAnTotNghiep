/************************************************
 *  @file     : app_alive.c
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "app_alive.h"
#include "peripheral/pio/plib_pio.h"
#include "M1_SysApp/xlog/xlog.h"
#include "M1_SysApp/dmesg/dmesg.h"
#include "stdio.h"
#include "M2_BSP/BSP_Led/bsp_led.h"
#include "M2_BSP/BSP_Watchdog/bsp_watchdog.h"


const osThreadAttr_t alive_attr = {
        .name = "LED_Alive",
        .stack_size = 256,
        .priority = configMAX_PRIORITIES - 4
};

void App_AliveTask(void *param)
{
    static uint8_t counter = 0;
    char buf[32];
    uint32_t last = Utils_GetTick();
    // xlog("\r\n");
    // xlog("\r\n");
    // xlog("\r\n");
    // xlog("\r\n");
    // xlog("\r\n");
    // xlog("========================================\r\n");
    // xlog("          _.-/`)            |V|\r\n");
    // xlog("         // / / )        .::| |::.\r\n");
    // xlog("      .=// / / / )      ::__| |__::\r\n");
    // xlog("     //`/ / / / /      >____   ____<\r\n");
    // xlog("    // /     ` /        ::  | |  ::\r\n");
    // xlog("   ||         /          '::| |::'\r\n");
    // xlog("    \\       /              | |\r\n");
    // xlog("     ))    .'               | |\r\n");
    // xlog("    //    /                 |A|\r\n");
    // xlog("   //    /                   |A|\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");
//    xlog(" ======== NEW FIRMWARE ========\r\n");

    for(;;)
    {
        bsp_watchdog_update(Utils_GetTick());
        if(Utils_GetTick() - last >= 2000)
        {
            last = Utils_GetTick();
            bsp_led_toggle();
            osThreadFeed();
            snprintf(buf, sizeof(buf), "Alive counter: %lu", counter++);
//            xlog("Hello World %d\r\n", counter++);
            Dmesg_Write(buf);
        }
        osDelay(pdMS_TO_TICKS(200));
    }
}
