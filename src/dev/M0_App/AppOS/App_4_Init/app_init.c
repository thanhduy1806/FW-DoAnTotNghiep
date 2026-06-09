#include "app_init.h"
#include "M1_SysApp/xlog/xlog.h"
#include "M1_SysApp/dmesg/dmesg.h"
#include "stdio.h"

#include "M2_BSP/BSP_Solenoid/bsp_solenoid.h"
#include "M2_BSP/BSP_RTC/bsp_rtc.h"
#include "M2_BSP/BSP_Pump/bsp_pump.h"
#include "M2_BSP/BSP_Flow_Sen/bsp_flow_sen.h"
#include "M2_BSP/BSP_TEC/bsp_tec.h"


const osThreadAttr_t init_attr = {
        .name = "Init",
        .stack_size = 256,
        .priority = configMAX_PRIORITIES - 2
};

void App_InitTask(void *param)
{
    (void)param;
    
    bsp_solenoid_init();
    bsp_rtc_init();
    bsp_flow_sen_init();
    bsp_pump_init();
    
    bsp_tec_init(p_tec[0]);
    bsp_tec_init(p_tec[2]);
    
    vTaskDelete(NULL);
}