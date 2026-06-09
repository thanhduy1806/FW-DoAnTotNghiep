/************************************************
 *  @file     : rgosh_error.c
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "rgosh_error.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

xerr_t code_error(int error)
{
    return (xerr_t)(-error);
}

const char * xerr_to_str(xerr_t err, char *buf, size_t buflen) {
    switch (err) {
        /* POSIX */
        case XERR_OK:                return "Success";
        case XERR_PERM:              return "Operation not permitted";
        case XERR_INTR:              return "Interrupted system call";
        case XERR_IO:                return "I/O error";
        case XERR_AGAIN:             return "Resource temporarily unavailable";
        case XERR_NOMEM:             return "Out of memory";
        case XERR_ACCES:             return "Permission denied";
        case XERR_BUSY:              return "Device or resource busy";
        case XERR_EXIST:             return "File or resource exists";
        case XERR_INVAL:             return "Invalid argument";
        case XERR_NOSYS:             return "Function not implemented";
        case XERR_OVERFLOW:          return "Value too large";
        case XERR_NOTSUP:            return "Operation not supported";
        case XERR_TIMEOUT:           return "Connection timed out";

        /* Custom */
        case XERR_HANDLE:            return "Invalid handle";
        case XERR_NOT_FOUND:         return "Resource not found";
        case XERR_FULL:              return "Buffer/Queue full";
        case XERR_RANGE:             return "Value out of range";
        case XERR_DATA:              return "Data corruption";
        case XERR_NO_DATA:           return "No data available";
        case XERR_STALE:             return "Data is stale";
        case XERR_TYPE:              return "Incompatible type";
        case XERR_STATE:             return "Invalid system state";
        case XERR_UNKNOWN:           return "Unknown error";

        default:
// #ifdef __MCU__
            if (buf != NULL && buflen > 0) {
                snprintf(buf, buflen, "Err: %d", (int)err);
                return buf;
            }
// #endif
            return "Undefined Error";
    }
}

/*
void vTaskError(void * pvParameters) {
    char err_buf[16]; 
    while(1) {
        xerr_t result = function();
        
        if (result != XERR_OK) {
            printf("Log: %s\n", xerr_to_str(result, err_buf, sizeof(err_buf)));
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
*/