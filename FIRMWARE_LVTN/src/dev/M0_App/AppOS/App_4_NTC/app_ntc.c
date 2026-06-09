#include "app_ntc.h"
#include "peripheral/pio/plib_pio.h"
#include "stdio.h"

#include "M1_SysApp/xlog/xlog.h"
#include "M1_SysApp/dmesg/dmesg.h"

#include "M2_BSP/BSP_Led/bsp_led.h"
#include "M2_BSP/BSP_NTC/bsp_ntc.h"

SemaphoreHandle_t ntc_mutex;

const osThreadAttr_t ntc_attr = {
    .name = "NTC",
    .stack_size = 256,
    .priority = configMAX_PRIORITIES - 4
};

void App_NTCTask(void *param) {
    (void) param;
    for (;;) {
        // Read raw values from hardware sensors
        float g_ntc_temp[8];
        bsp_ntc_get_temp(g_ntc_temp);

        // Protect access to db_temperature
        if (xSemaphoreTake(ntc_mutex, portMAX_DELAY) == pdTRUE) {
            
            // Loop through all NTC sensors (NTC_1 to NTC_8)
            for (int i = 0; i < 8; i++) {
                // Use the write function to handle conversion and storage
                db_temperature_write((e_db_temperature_t)i, (float)g_ntc_temp[i]);
            }
            
            // If you have the board temperature at the end of the array
            // db_temperature_write(BOARD_TEMP, g_board_temp_raw);

            xSemaphoreGive(ntc_mutex);
        }

        osDelay(pdMS_TO_TICKS(100));
    }
}

void ntc_init(void) {
    ntc_mutex = xSemaphoreCreateMutex();

    if (ntc_mutex == NULL) {
        xlog("Create NTC mutex failed\r\n");
    }
}