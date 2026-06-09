#include "evt.h"
#include <string.h>

/* ============================================================
 *  Internal — async state
 * ============================================================ */
static QueueHandle_t s_queue = NULL;
static evt_group_t  *s_group = NULL;

/* ============================================================
 *  Internal — dispatch sync
 * ============================================================ */
static void _dispatch(evt_group_t *grp, evt_t *evt) {
    for (uint8_t i = 0; i < grp->count; i++) {
        if (grp->handlers[i]) {
            grp->handlers[i](evt);
        }
    }
}

/* ============================================================
 *  SYNC implementation
 * ============================================================ */
void EVT_CreateGroup(evt_group_t *grp) {
    if (grp) memset(grp, 0, sizeof(*grp));
}

bool EVT_AddHandler_toGroup(evt_group_t *grp, evt_handler_t handler) {
    if (!grp || !handler || grp->count >= EVT_MAX_HANDLERS) return false;
    grp->handlers[grp->count++] = handler;
    return true;
}

void EVT_Emit(evt_group_t *grp, uint16_t type) {
    if (!grp) return;
    evt_t e = { .type = type, .table_id = 0xFF, ._pad = 0, .msg.data = 0 };
    _dispatch(grp, &e);
}

void EVT_EmitAdvanced(evt_group_t *grp, evt_t *evt) {
    if (!grp || !evt) return;
    _dispatch(grp, evt);
}

void EVT_Broadcast(evt_group_t *groups, uint8_t count, evt_t *evt) {
    if (!groups || !evt) return;
    for (uint8_t i = 0; i < count; i++) _dispatch(&groups[i], evt);
}

/* ============================================================
 *  ASYNC implementation
 * ============================================================ */
void EVT_AsyncBind(QueueHandle_t queue, evt_group_t *grp) {
    s_queue = queue;
    s_group = grp;
}

bool EVT_AsyncProcess(TickType_t wait_ticks) {
    if (!s_queue || !s_group) return false;
    evt_t evt;
    if (xQueueReceive(s_queue, &evt, wait_ticks) == pdTRUE) {
        _dispatch(s_group, &evt);
        return true;
    }
    return false;
}

bool EVT_Post(evt_t *evt) {
    if (!s_queue || !evt) return false;
    return xQueueSend(s_queue, evt, 0) == pdTRUE;
}

bool EVT_PostFromISR(evt_t *evt) {
    if (!s_queue || !evt) return false;
    BaseType_t woken = pdFALSE;
    bool ok = xQueueSendFromISR(s_queue, evt, &woken) == pdTRUE;
    portYIELD_FROM_ISR(woken);
    return ok;
}