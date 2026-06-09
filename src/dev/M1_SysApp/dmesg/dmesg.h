/************************************************
 *  @file     : dmesg.h
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 2.0.0
 *-----------------------------------------------
 *  Description :
 ************************************************/

#ifndef dmesg_H
#define dmesg_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"

/* Configuration */
#define DMESG_BUFFER_SIZE       (1U * 256U)  /* 16KB total buffer */
#define DMESG_MSG_MAX_LENGTH    (128U)         /* Max message length */

/* Error codes */
typedef enum {
    DMESG_OK = 0,
    DMESG_ERROR_NULL_POINTER,
    DMESG_ERROR_INIT_FAILED,
    DMESG_ERROR_BUFFER_FULL,
    DMESG_ERROR_INVALID_LENGTH,
    DMESG_ERROR_CORRUPTED_ENTRY,
    DMESG_ERROR_NOT_INITIALIZED
} Dmesg_Error_t;

/* Statistics structure */
typedef struct {
    uint32_t total_writes;
    uint32_t dropped_messages;
    uint32_t current_entries;
    uint32_t buffer_utilization_percent;
    uint32_t max_entries_reached;
} Dmesg_Stats_t;

/* Note that!!! : Caller must ensure thread safety */
Dmesg_Error_t Dmesg_Init(void);
bool Dmesg_IsInitialized(void);

Dmesg_Error_t Dmesg_Write(const char *msg);
Dmesg_Error_t Dmesg_WriteISR(const char *msg); 

void Dmesg_GetLogs(void);
void Dmesg_GetLatestN(size_t N);
Dmesg_Error_t Dmesg_Clear(void);

Dmesg_Stats_t Dmesg_GetStats(void);

#ifdef __cplusplus
}
#endif

#endif /* dmesg_H */