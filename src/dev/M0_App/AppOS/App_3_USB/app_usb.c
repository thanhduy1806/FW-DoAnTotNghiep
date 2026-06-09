/************************************************
 *  @file     : app_usb.c
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "app_usb.h"
#include "tusb.h"
#include "board.h"

/* ----------------- USB Task ----------------- */
const osThreadAttr_t usb_attr = {
        .name = "USB_Task",
        .stack_size = 4096,
        .priority = configMAX_PRIORITIES - 3
};

void App_USBTask(void *param)
{
    tusb_rhport_init_t dev_init = {
        .role  = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_HIGH
    };
    tusb_init(0, &dev_init);

    for(;;)
    {
        tud_task();
        osThreadFeed();
        osDelay(pdMS_TO_TICKS(1));
    }
}