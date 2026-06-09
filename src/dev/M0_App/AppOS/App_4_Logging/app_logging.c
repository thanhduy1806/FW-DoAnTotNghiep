/************************************************
 *  @file     : app_logging.c
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "M0_App/OS/os.h"
#include "M1_SysApp/xlog/xlog.h"
#include "app_logging.h"
#include "stream_buffer.h"
#include "M2_BSP/UART/uart_irq.h"

#define USE_RTOS   

const osThreadAttr_t log_attr = {
        .name = "Log_Task",
        .stack_size = 256,
        .priority = configMAX_PRIORITIES - 4
};

void App_LogTask(void *argument)
{
    uint8_t tx_buf[64];
    size_t received_bytes;

#ifdef USE_RTOS
    uint16_t tx_count = 0;
#endif

    for (;;)
    {
        received_bytes = xStreamBufferReceive(
                            xLogStreamBuffer,
                            tx_buf,
                            sizeof(tx_buf),
                            pdMS_TO_TICKS(100));

        osThreadFeed();

        if (received_bytes > 0)
        {
            for (size_t i = 0; i < received_bytes; i++)
            {
                UART2_WriteByte(tx_buf[i]);

#ifdef USE_RTOS
                tx_count++;

                if (tx_count >= 127)
                {
                    tx_count = 0;
                    vTaskDelay(10); 
                }
#endif
            }
        }
    }
}