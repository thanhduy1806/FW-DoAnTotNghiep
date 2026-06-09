/* ============================================================
 * app_ctrl.h
 * ============================================================ */
#ifndef APP_CTRL_H
#define APP_CTRL_H

#include "os.h"
#include "M1_SysApp/evt/evt.h"
#include "exp_rparam_table.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------ Event types Table 2 (Manual-Control) ------ */
#define CTRL_EVT_SLN_CTRL    0x10
#define CTRL_EVT_SLN_MODE    0x11
#define CTRL_EVT_SL_CTRL     0x18
#define CTRL_EVT_SL_MODE     0x19
#define CTRL_EVT_HD4_CTRL    0x20
#define CTRL_EVT_HD4_MODE    0x21
#define CTRL_EVT_VAL_CFG     0x28
#define CTRL_EVT_VAL_CTRL    0x29
#define CTRL_EVT_TEC_TEMP    0x30
#define CTRL_EVT_TEC_MODE    0x31
#define CTRL_EVT_HTER_TEMP   0x40
#define CTRL_EVT_HTER_MODE   0x41
#define CTRL_EVT_PDLS_MODE   0x50

extern evt_group_t g_ctrl_group;

extern const osThreadAttr_t ctrl_attr;
void App_CtrlTask(void *param);

void App_Ctrl_Table2_CB(uint16_t addr, gs_param_table_instance_t *tinst);

#ifdef __cplusplus
}
#endif
#endif /* APP_CTRL_H */