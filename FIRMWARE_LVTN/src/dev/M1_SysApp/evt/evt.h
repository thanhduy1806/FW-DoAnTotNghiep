#ifndef EVT_H
#define EVT_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVT_MAX_HANDLERS     8  
#define EVT_QUEUE_LEN       64  

typedef union {
    uint32_t data;
    void    *ptr;  
} evt_msg_t;

typedef struct {
    uint16_t  type;  
    uint8_t   table_id;
    uint8_t   _pad;
    evt_msg_t msg;
} evt_t;

typedef bool (*evt_handler_t)(evt_t *evt);

typedef struct {
    evt_handler_t handlers[EVT_MAX_HANDLERS];
    uint8_t       count;
} evt_group_t;

/* ============================================================
 *  Macro
 * ============================================================ */
#define EVT_PACK(idx, val)  (((uint32_t)(idx) << 16) | ((uint32_t)(val) & 0xFFFF))
#define EVT_IDX(data)       ((uint16_t)((data) >> 16))
#define EVT_VAL16(data)     ((uint16_t)((data) & 0xFFFF))
#define EVT_VAL8(data)      ((uint8_t) ((data) & 0xFF))

/* ============================================================
 *  SYNC API
 * ============================================================ */
void EVT_CreateGroup(evt_group_t *grp);
bool EVT_AddHandler_toGroup(evt_group_t *grp, evt_handler_t handler);
void EVT_Emit(evt_group_t *grp, uint16_t type);
void EVT_EmitAdvanced(evt_group_t *grp, evt_t *evt);
void EVT_Broadcast(evt_group_t *groups, uint8_t count, evt_t *evt);

/* ============================================================
 *  ASYNC API 
 * ============================================================ */
bool EVT_Post(evt_t *evt);
bool EVT_PostFromISR(evt_t *evt);
void EVT_AsyncBind(QueueHandle_t queue, evt_group_t *grp);
bool EVT_AsyncProcess(TickType_t wait_ticks);

#ifdef __cplusplus
}
#endif
#endif /* EVT_H */