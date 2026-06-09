/************************************************
 *  @file     : rgosh_error.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef RGOSH_ERROR_H
#define RGOSH_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef enum {
    /* --- POSIX CODES --- */
    XERR_OK              = 0,
    XERR_PERM            = -1,   /* Operation not permitted */
    XERR_INTR            = -4,   /* Interrupted system call */
    XERR_IO              = -5,   /* I/O error */
    XERR_AGAIN           = -11,  /* Try again */
    XERR_NOMEM           = -12,  /* Out of memory */
    XERR_ACCES           = -13,  /* Permission denied */
    XERR_BUSY            = -16,  /* Device or resource busy */
    XERR_EXIST           = -17,  /* File/Resource exists */
    XERR_INVAL           = -22,  /* Invalid argument */
    XERR_NOSYS           = -38,  /* Function not implemented */
    XERR_OVERFLOW        = -75,  /* Value too large */
    XERR_NOTSUP          = -95,  /* Operation not supported */
    XERR_TIMEOUT         = -110, /* Connection timed out */

    /* --- CUSTOM CODES --- */
    XERR_CUSTOM_BASE     = -200,
    XERR_HANDLE          = -200, /* Invalid handle or pointer */
    XERR_NOT_FOUND       = -201, /* Resource not found */
    XERR_FULL            = -202, /* Buffer or queue is full */
    XERR_RANGE           = -203, /* Value out of range */
    XERR_DATA            = -204, /* Data corruption or invalid format */
    XERR_NO_DATA         = -205, /* No data available */
    XERR_STALE           = -206, /* Data is old/not updated */
    XERR_TYPE            = -207, /* Incompatible type */
    XERR_STATE           = -208, /* Invalid system state */
    XERR_UNKNOWN         = -255  /* Catch-all error code */
} xerr_t;

const char * xerr_to_str(xerr_t err, char *buf, size_t buflen);

xerr_t xcode_error(int error);



#ifdef __cplusplus
}
#endif

#endif /* RGOSH_ERROR_H */