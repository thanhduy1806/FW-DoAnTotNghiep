/************************************************
 *  @file     : xlog.c
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "M0_App/OS/os.h"
#include "M1_SysApp/xlog/xlog.h"
#include "stream_buffer.h"
#include "semphr.h" 
#include <stdarg.h>
#include <stdio.h>

#define LOG_BUFFER_SIZE 4096
static uint8_t ucLogStorage[LOG_BUFFER_SIZE];
static StaticStreamBuffer_t xStreamBufferStruct;
SemaphoreHandle_t xLogMutex;
StreamBufferHandle_t xLogStreamBuffer;

void xlog_Init(void) {
    xLogStreamBuffer = xStreamBufferCreateStatic(
        LOG_BUFFER_SIZE,
        1,
        ucLogStorage,
        &xStreamBufferStruct
    );
    xLogMutex = xSemaphoreCreateMutex();
}

void xlog(const char *fmt, ...) {
    char buf[256];
    va_list args;
    
    if (xSemaphoreTake(xLogMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        va_start(args, fmt);
        int len = vsnprintf(buf, sizeof(buf), fmt, args); 
        va_end(args);

        if (len > 0) {
            xStreamBufferSend(xLogStreamBuffer, buf, len, 0); 
        }
        xSemaphoreGive(xLogMutex);
    }
}