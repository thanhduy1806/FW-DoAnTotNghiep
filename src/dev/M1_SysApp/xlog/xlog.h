/************************************************
 *  @file     : xlog.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#ifndef xlog_H
#define xlog_H


#include "M0_App/OS/os.h"
#include "stream_buffer.h"

void xlog_Init(void);

extern StreamBufferHandle_t xLogStreamBuffer;
void xlog(const char *fmt, ...);

#endif /* xlog_H */