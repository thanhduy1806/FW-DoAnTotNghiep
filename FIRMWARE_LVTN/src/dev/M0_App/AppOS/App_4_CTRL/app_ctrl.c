/* ============================================================
 * app_ctrl.c
 * ============================================================ */
#include "app_ctrl.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "M1_SysApp/dmesg/dmesg.h"
#include <string.h>
#include <param/types.h>
#include "param/internal/types.h"
#include <param/table.h>

#include "M1_SysApp/xlog/xlog.h"

/* ============================================================
 *  Internal
 * ============================================================ */
evt_group_t g_ctrl_group;
extern gs_param_table_instance_t g_tables[5];
static QueueHandle_t s_ctrl_queue = NULL;

uint32_t counter = 0;

/* ============================================================
 *  Table 2 callback
 * ============================================================ */
void App_Ctrl_Table2_CB(uint16_t addr, gs_param_table_instance_t *tinst) {
    evt_t e;
    e.table_id = 2;
    e._pad     = 0;
    e.msg.data = 0;

    xlog("Callback with: %d\r\n", counter++);

    /* sln_mode */
    if (addr == PARAM_T2_SLN_MODE) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_SLN_MODE; e.msg.data = v;
        xlog("Post event: type=0x%02X (SLN_MODE), data=0x%02X\r\n", e.type, e.msg.data);
        EVT_Post(&e); return;
    }
    /* sln_ctrl[0..3] */
    if (addr >= PARAM_T2_SLN_CTRL && addr < PARAM_T2_SLN_CTRL + 4) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_SLN_CTRL;
        e.msg.data = EVT_PACK((uint8_t)(addr - PARAM_T2_SLN_CTRL), v);
        xlog("Post event: type=0x%02X (SLN_CTRL), idx=%u, val=0x%02X\r\n",
             e.type, (uint8_t)(addr - PARAM_T2_SLN_CTRL), v);
        EVT_Post(&e); return;
    }
    /* sl_mode */
    if (addr == PARAM_T2_SL_MODE) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_SL_MODE; e.msg.data = v;
        xlog("Post event: type=0x%02X (SL_MODE), data=0x%02X\r\n", e.type, e.msg.data);
        EVT_Post(&e); return;
    }
    /* sl_ctrl1[0..7] */
    if (addr >= PARAM_T2_SL_CTRL1 && addr < PARAM_T2_SL_CTRL1 + 8) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_SL_CTRL;
        e.msg.data = EVT_PACK((uint8_t)(addr - PARAM_T2_SL_CTRL1), v);
        xlog("Post event: type=0x%02X (SL_CTRL1), idx=%u, val=0x%02X\r\n",
             e.type, (uint8_t)(addr - PARAM_T2_SL_CTRL1), v);
        EVT_Post(&e); return;
    }
    /* sl_ctrl2[0..7] — index offset by 8 to distinguish from sl_ctrl1 */
    if (addr >= PARAM_T2_SL_CTRL2 && addr < PARAM_T2_SL_CTRL2 + 8) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_SL_CTRL;
        e.msg.data = EVT_PACK((uint8_t)(8 + (addr - PARAM_T2_SL_CTRL2)), v);
        xlog("Post event: type=0x%02X (SL_CTRL2), idx=%u, val=0x%02X\r\n",
             e.type, (uint8_t)(8 + (addr - PARAM_T2_SL_CTRL2)), v);
        EVT_Post(&e); return;
    }
    /* hd4_mode */
    if (addr == PARAM_T2_HD4_MODE) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_HD4_MODE; e.msg.data = v;
        xlog("Post event: type=0x%02X (HD4_MODE), data=0x%02X\r\n", e.type, e.msg.data);
        EVT_Post(&e); return;
    }
    /* hd4_ctrl[0..7] */
    if (addr >= PARAM_T2_HD4_CTRL && addr < PARAM_T2_HD4_CTRL + 8) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_HD4_CTRL;
        e.msg.data = EVT_PACK((uint8_t)(addr - PARAM_T2_HD4_CTRL), v);
        xlog("Post event: type=0x%02X (HD4_CTRL), idx=%u, val=0x%02X\r\n",
             e.type, (uint8_t)(addr - PARAM_T2_HD4_CTRL), v);
        EVT_Post(&e); return;
    }
    /* val_cfg */
    if (addr == PARAM_T2_VAL_CFG) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_VAL_CFG; e.msg.data = v;
        xlog("Post event: type=0x%02X (VAL_CFG), data=0x%02X\r\n", e.type, e.msg.data);
        EVT_Post(&e); return;
    }
    /* val_ctrl */
    if (addr == PARAM_T2_VAL_CTRL) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_VAL_CTRL; e.msg.data = v;
        xlog("Post event: type=0x%02X (VAL_CTRL), data=0x%02X\r\n", e.type, e.msg.data);
        EVT_Post(&e); return;
    }
    /* tec_mode */
    if (addr == PARAM_T2_TEC_MODE) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_TEC_MODE; e.msg.data = v;
        xlog("Post event: type=0x%02X (TEC_MODE), data=0x%02X\r\n", e.type, e.msg.data);
        EVT_Post(&e); return;
    }
    /* tec_temp[0..3] */
    if (addr >= PARAM_T2_TEC_TEMP &&
        addr <  PARAM_T2_TEC_TEMP + 4 * (uint16_t)sizeof(int16_t)) {
        int16_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_INT16, &v, sizeof(v), 0);
        e.type = CTRL_EVT_TEC_TEMP;
        e.msg.data = EVT_PACK((uint8_t)((addr - PARAM_T2_TEC_TEMP) / sizeof(int16_t)), (uint16_t)v);
        xlog("Post event: type=0x%02X (TEC_TEMP), idx=%u, temp=%d\r\n",
             e.type, (uint8_t)((addr - PARAM_T2_TEC_TEMP) / sizeof(int16_t)), v);
        EVT_Post(&e); return;
    }
    /* hter_mode */
    if (addr == PARAM_T2_HTER_MODE) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_HTER_MODE; e.msg.data = v;
        xlog("Post event: type=0x%02X (HTER_MODE), data=0x%02X\r\n", e.type, e.msg.data);
        EVT_Post(&e); return;
    }
    /* hter_temp[0..7] */
    if (addr >= PARAM_T2_HTER_TEMP &&
        addr <  PARAM_T2_HTER_TEMP + 8 * (uint16_t)sizeof(uint16_t)) {
        uint16_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT16, &v, sizeof(v), 0);
        e.type = CTRL_EVT_HTER_TEMP;
        e.msg.data = EVT_PACK((uint8_t)((addr - PARAM_T2_HTER_TEMP) / sizeof(uint16_t)), v);
        xlog("Post event: type=0x%02X (HTER_TEMP), idx=%u, temp=%u\r\n",
             e.type, (uint8_t)((addr - PARAM_T2_HTER_TEMP) / sizeof(uint16_t)), v);
        EVT_Post(&e); return;
    }
    /* pdls_mode (Table 3, nhưng callback vẫn từ T2 context) */
    if (addr == PARAM_T3_PDLS_MODE) {
        uint8_t v = 0;
        gs_param_get(tinst, addr, GS_PARAM_UINT8, &v, 1, 0);
        e.type = CTRL_EVT_PDLS_MODE; e.msg.data = v;
        xlog("Post event: type=0x%02X (PDLS_MODE), data=0x%02X\r\n", e.type, e.msg.data);
        EVT_Post(&e); return;
    }
}

/* ============================================================
 *  Handlers
 * ============================================================ */
static bool Sol_Handler(evt_t *e) {
    if (e->type == CTRL_EVT_SLN_MODE) {
        xlog("Sol_Handler: SLN_MODE=0x%02X\r\n", EVT_VAL8(e->msg.data));
        // Sol_SetMode(EVT_VAL8(e->msg.data));
        return true;
    }
    if (e->type == CTRL_EVT_SLN_CTRL) {
        uint8_t idx = EVT_IDX(e->msg.data);
        uint8_t val = EVT_VAL8(e->msg.data);
        xlog("Sol_Handler: SLN_CTRL idx=%u, val=0x%02X\r\n", idx, val);
        // Sol_Set(idx, val);
        (void)idx; (void)val;
        return true;
    }
    return false;
}

static bool Sl_Handler(evt_t *e) {
    if (e->type == CTRL_EVT_SL_MODE) {
        xlog("Sl_Handler: SL_MODE=0x%02X\r\n", EVT_VAL8(e->msg.data));
        // Sl_SetMode(EVT_VAL8(e->msg.data));
        return true;
    }
    if (e->type == CTRL_EVT_SL_CTRL) {
        uint8_t idx = EVT_IDX(e->msg.data);   // 0..7 = gr1, 8..15 = gr2
        uint8_t val = EVT_VAL8(e->msg.data);
        xlog("Sl_Handler: SL_CTRL idx=%u, val=0x%02X\r\n", idx, val);
        // Sl_Set(idx, val);
        (void)idx; (void)val;
        return true;
    }
    return false;
}

static bool Hd4_Handler(evt_t *e) {
    if (e->type == CTRL_EVT_HD4_MODE) {
        xlog("Hd4_Handler: HD4_MODE=0x%02X\r\n", EVT_VAL8(e->msg.data));
        // Hd4_SetMode(EVT_VAL8(e->msg.data));
        return true;
    }
    if (e->type == CTRL_EVT_HD4_CTRL) {
        uint8_t idx = EVT_IDX(e->msg.data);
        uint8_t val = EVT_VAL8(e->msg.data);
        xlog("Hd4_Handler: HD4_CTRL idx=%u, val=0x%02X\r\n", idx, val);
        // Hd4_Set(idx, val);
        (void)idx; (void)val;
        return true;
    }
    return false;
}

static bool Val_Handler(evt_t *e) {
    if (e->type == CTRL_EVT_VAL_CFG) {
        xlog("Val_Handler: VAL_CFG=0x%02X\r\n", EVT_VAL8(e->msg.data));
        // Val_SetCfg(EVT_VAL8(e->msg.data));
        return true;
    }
    if (e->type == CTRL_EVT_VAL_CTRL) {
        xlog("Val_Handler: VAL_CTRL=0x%02X\r\n", EVT_VAL8(e->msg.data));
        // Val_SetCtrl(EVT_VAL8(e->msg.data));
        return true;
    }
    return false;
}

static bool Tec_Handler(evt_t *e) {
    if (e->type == CTRL_EVT_TEC_MODE) {
        xlog("Tec_Handler: TEC_MODE=0x%02X\r\n", EVT_VAL8(e->msg.data));
        // Tec_SetMode(EVT_VAL8(e->msg.data));
        return true;
    }
    if (e->type == CTRL_EVT_TEC_TEMP) {
        uint8_t idx  = EVT_IDX(e->msg.data);
        int16_t temp = (int16_t)EVT_VAL16(e->msg.data);
        xlog("Tec_Handler: TEC_TEMP idx=%u, temp=%d\r\n", idx, temp);
        // Tec_SetTarget(idx, temp);
        (void)idx; (void)temp;
        return true;
    }
    return false;
}

static bool Hter_Handler(evt_t *e) {
    if (e->type == CTRL_EVT_HTER_MODE) {
        xlog("Hter_Handler: HTER_MODE=0x%02X\r\n", EVT_VAL8(e->msg.data));
        // Hter_SetMode(EVT_VAL8(e->msg.data));
        return true;
    }
    if (e->type == CTRL_EVT_HTER_TEMP) {
        uint8_t  idx  = EVT_IDX(e->msg.data);
        uint16_t temp = EVT_VAL16(e->msg.data);
        xlog("Hter_Handler: HTER_TEMP idx=%u, temp=%u\r\n", idx, temp);
        // Hter_SetTarget(idx, temp);
        (void)idx; (void)temp;
        return true;
    }
    return false;
}

static bool Pdls_Handler(evt_t *e) {
    if (e->type == CTRL_EVT_PDLS_MODE) {
        xlog("Pdls_Handler: PDLS_MODE=0x%02X\r\n", EVT_VAL8(e->msg.data));
        // Pdls_SetMode(EVT_VAL8(e->msg.data));
        return true;
    }
    return false;
}

/* ============================================================
 *  App_CtrlTask
 * ============================================================ */
const osThreadAttr_t ctrl_attr = {
    .name       = "Ctrl",
    .stack_size = configMINIMAL_STACK_SIZE * 2,
    .priority   = configMAX_PRIORITIES - 4
};

void App_CtrlTask(void *param) {
    (void)param;

    /* 1. Create queue */
    s_ctrl_queue = xQueueCreate(EVT_QUEUE_LEN, sizeof(evt_t));
    configASSERT(s_ctrl_queue);

    /* 2. Create event group & register handlers */
    EVT_CreateGroup(&g_ctrl_group);
    EVT_AddHandler_toGroup(&g_ctrl_group, Sol_Handler);
    EVT_AddHandler_toGroup(&g_ctrl_group, Sl_Handler);
    EVT_AddHandler_toGroup(&g_ctrl_group, Hd4_Handler);
    EVT_AddHandler_toGroup(&g_ctrl_group, Val_Handler);
    EVT_AddHandler_toGroup(&g_ctrl_group, Tec_Handler);
    EVT_AddHandler_toGroup(&g_ctrl_group, Hter_Handler);
    EVT_AddHandler_toGroup(&g_ctrl_group, Pdls_Handler);

    /* 3. Bind queue -> EVT async system */
    EVT_AsyncBind(s_ctrl_queue, &g_ctrl_group);

    /* 4. Bind callback to Table 2 */
    g_tables[2].callback = App_Ctrl_Table2_CB;

    Dmesg_Write("[CTRL] Task ready");

    /* 5. Event loop */
    for (;;) {
        EVT_AsyncProcess(portMAX_DELAY);
    }
}