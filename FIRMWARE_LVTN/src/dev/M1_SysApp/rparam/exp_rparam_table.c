#include "exp_rparam_table.h"
#include "param/internal/types.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================================
 * Table 0: Board Parameter
 * ============================================================================ */

typedef struct __attribute__((packed)) {
    char uid[16];                      // 0
    char type[8];                     // 16
    uint8_t rev[3];                         // 24
    uint8_t csp_addr;                      // 27
    char csp_port[16];                 // 28
    char csp_rtable[96];               // 44
    uint16_t can_speed;                     // 140
    uint16_t reboot_in;                     // 142
    uint16_t default_in;                    // 144
} table_data_t0;

static table_data_t0 g_data_t0 = {
    .uid = "CM-EXP",
    .type = "C",
    .rev = {1, 0, 0},
    .csp_addr = 11,
    .csp_port = "7,8,12",
    .csp_rtable = {""},
    .can_speed = 1000,
    .reboot_in = 0,
    .default_in = 0,
};

const gs_param_table_row_t data_table0_rows[] = {
    {.addr = PARAM_T0_UID, .type = GS_PARAM_STRING, .size = 16,
     .array_size = 1, .flags = 0, .name = "uid"},
    {.addr = PARAM_T0_TYPE, .type = GS_PARAM_STRING, .size = 8,
     .array_size = 1, .flags = 0, .name = "type"},
    {.addr = PARAM_T0_REV, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 3, .flags = 0, .name = "rev"},
    {.addr = PARAM_T0_CSP_ADDR, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "csp_addr"},
    {.addr = PARAM_T0_CSP_PORT, .type = GS_PARAM_STRING, .size = 16,
     .array_size = 1, .flags = 0, .name = "csp_port"},
    {.addr = PARAM_T0_CSP_RTABLE, .type = GS_PARAM_STRING, .size = 96,
     .array_size = 1, .flags = 0, .name = "csp_rtable"},
    {.addr = PARAM_T0_CAN_SPEED, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "can_speed"},
    {.addr = PARAM_T0_REBOOT_IN, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "reboot_in"},
    {.addr = PARAM_T0_DEFAULT_IN, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "default_in"},
};

const size_t data_table0_row_count = sizeof(data_table0_rows) / sizeof(data_table0_rows[0]);

/* ============================================================================
 * Table 1: Sys-config Parameter
 * ============================================================================ */

typedef struct __attribute__((packed)) {
    uint32_t time_set;                      // 0
    char power_name[7][4];     // 4
    uint8_t power_set[7];                   // 32
    char i2c_name[6][4];       // 39
    uint8_t i2c_set[6];                     // 63
    uint8_t led_en;                        // 69
    uint8_t ifb_en;                        // 70
    uint8_t mpu_en;                        // 71
    uint8_t ota_status;                    // 72
    uint8_t ota_ctrl;                      // 73
} table_data_t1;

static table_data_t1 g_data_t1 = {
    .time_set = 0,
    .power_name = {"som", "peri", "tec", "hd4", "sln", "lspd", "hter"},
    .power_set = {1, 1, 0, 0, 0, 0, 0},
    .i2c_name = {"sen1", "sen2", "sen3", "sen4", "sen5", "hd4"},
    .i2c_set = {0, 0, 0, 0, 0, 0},
    .led_en = 1,
    .ifb_en = 0,
    .mpu_en = 1,
    .ota_status = 0,
    .ota_ctrl = 0,
};

const gs_param_table_row_t data_table1_rows[] = {
    {.addr = PARAM_T1_TIME_SET, .type = GS_PARAM_UINT32, .size = sizeof(uint32_t),
     .array_size = 1, .flags = 0, .name = "time_set"},
    {.addr = PARAM_T1_POWER_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 7, .flags = 0, .name = "power_name"},
    {.addr = PARAM_T1_POWER_SET, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 7, .flags = 0, .name = "power_set"},
    {.addr = PARAM_T1_I2C_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 6, .flags = 0, .name = "i2c_name"},
    {.addr = PARAM_T1_I2C_SET, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 6, .flags = 0, .name = "i2c_set"},
    {.addr = PARAM_T1_LED_EN, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "led_en"},
    {.addr = PARAM_T1_IFB_EN, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "ifb_en"},
    {.addr = PARAM_T1_MPU_EN, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "mpu_en"},
    {.addr = PARAM_T1_OTA_STATUS, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "ota_status"},
    {.addr = PARAM_T1_OTA_CTRL, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "ota_ctrl"},
};

const size_t data_table1_row_count = sizeof(data_table1_rows) / sizeof(data_table1_rows[0]);

/* ============================================================================
 * Table 2: Manual-Control Parameter
 * ============================================================================ */

typedef struct __attribute__((packed)) {
    uint8_t sig_mpu;                       // 0
    char sln_name[4][4];       // 1
    uint8_t sln_mode;                      // 17
    uint8_t sln_ctrl[4];                    // 18
    char sl_gr1[8][4];         // 22
    char sl_gr2[8][4];         // 54
    uint8_t sl_mode;                       // 86
    uint8_t sl_ctrl1[8];                    // 87
    uint8_t sl_ctrl2[8];                    // 95
    char hd4_name[8][4];       // 103
    uint8_t hd4_mode;                      // 135
    uint8_t hd4_ctrl[8];                    // 136
    uint8_t val_cfg;                       // 144
    uint8_t val_ctrl;                      // 145
    char tec_name[4][4];       // 146
    uint8_t tec_mode;                      // 162
    int16_t tec_temp[4];                    // 163
    char hter_name[8][4];      // 171
    uint8_t hter_mode;                     // 203
    uint16_t hter_temp[8];                   // 204
    char ctrl_status[16];              // 220
} table_data_t2;

static table_data_t2 g_data_t2 = {
    .sig_mpu = 0,
    .sln_name = {"sol1", "sol2", "sol3", "sol4"},
    .sln_mode = 0,
    .sln_ctrl = {0, 0, 0, 0},
    .sl_gr1 = {"sl1", "sl2", "sl3", "sl4", "sl5", "sl6", "sl7", "sl8"},
    .sl_gr2 = {"sl9", "sl10", "sl11", "sl12", "sl13", "sl14", "sl15", "sl16"},
    .sl_mode = 0,
    .sl_ctrl1 = {0, 0, 0, 0, 0, 0, 0, 0},
    .sl_ctrl2 = {0, 0, 0, 0, 0, 0, 0, 0},
    .hd4_name = {"ch11", "ch12", "ch21", "ch22", "ch31", "ch32", "ch41", "ch42"},
    .hd4_mode = 0,
    .hd4_ctrl = {0, 0, 0, 0, 0, 0, 0, 0},
    .val_cfg = 0,
    .val_ctrl = 0,
    .tec_name = {"tec1", "tec2", "tec3", "tec4"},
    .tec_mode = 0,
    .tec_temp = {0, 0, 0, 0},
    .hter_name = {"ht1", "ht2", "ht3", "ht4", "ht5", "ht6", "ht7", "ht8"},
    .hter_mode = 0,
    .hter_temp = {0, 0, 0, 0, 0, 0, 0, 0},
    .ctrl_status = "OK",
};

const gs_param_table_row_t data_table2_rows[] = {
    {.addr = PARAM_T2_SIG_MPU, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "sig_mpu"},
    {.addr = PARAM_T2_SLN_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 4, .flags = 0, .name = "sln_name"},
    {.addr = PARAM_T2_SLN_MODE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "sln_mode"},
    {.addr = PARAM_T2_SLN_CTRL, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 4, .flags = 0, .name = "sln_ctrl"},
    {.addr = PARAM_T2_SL_GR1, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 8, .flags = 0, .name = "sl_gr1"},
    {.addr = PARAM_T2_SL_GR2, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 8, .flags = 0, .name = "sl_gr2"},
    {.addr = PARAM_T2_SL_MODE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "sl_mode"},
    {.addr = PARAM_T2_SL_CTRL1, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 8, .flags = 0, .name = "sl_ctrl1"},
    {.addr = PARAM_T2_SL_CTRL2, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 8, .flags = 0, .name = "sl_ctrl2"},
    {.addr = PARAM_T2_HD4_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 8, .flags = 0, .name = "hd4_name"},
    {.addr = PARAM_T2_HD4_MODE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "hd4_mode"},
    {.addr = PARAM_T2_HD4_CTRL, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 8, .flags = 0, .name = "hd4_ctrl"},
    {.addr = PARAM_T2_VAL_CFG, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "val_cfg"},
    {.addr = PARAM_T2_VAL_CTRL, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "val_ctrl"},
    {.addr = PARAM_T2_TEC_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 4, .flags = 0, .name = "tec_name"},
    {.addr = PARAM_T2_TEC_MODE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "tec_mode"},
    {.addr = PARAM_T2_TEC_TEMP, .type = GS_PARAM_INT16, .size = sizeof(int16_t),
     .array_size = 4, .flags = 0, .name = "tec_temp"},
    {.addr = PARAM_T2_HTER_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 8, .flags = 0, .name = "hter_name"},
    {.addr = PARAM_T2_HTER_MODE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "hter_mode"},
    {.addr = PARAM_T2_HTER_TEMP, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 8, .flags = 0, .name = "hter_temp"},
    {.addr = PARAM_T2_CTRL_STATUS, .type = GS_PARAM_STRING, .size = 16,
     .array_size = 1, .flags = 0, .name = "ctrl_status"},
};

const size_t data_table2_row_count = sizeof(data_table2_rows) / sizeof(data_table2_rows[0]);

/* ============================================================================
 * Table 3: Experiment Parameter
 * ============================================================================ */

typedef struct __attribute__((packed)) {
    uint16_t dls_inten;                     // 0
    uint16_t dls_rate;                      // 2
    uint16_t dls_pre;                       // 4
    uint16_t dls_samp;                      // 6
    uint16_t dls_post;                      // 8
    uint8_t seq_mode;                      // 10
    uint8_t seq_ctrl;                      // 11
    uint8_t exp_mode;                      // 12
    uint8_t exp_ctrl;                      // 13
    char pdls_rw1[8][4];       // 14
    char pdls_rw2[8][4];       // 46
    char pdls_rw3[8][4];       // 78
    uint8_t pdls_mode;                     // 110
    uint16_t pdls_vl1[8];                    // 111
    uint16_t pdls_vl2[8];                    // 127
    uint16_t pdls_vl3[8];                    // 143
    uint16_t pdls_cr1[8];                    // 159
    uint16_t pdls_cr2[8];                    // 175
    uint16_t pdls_cr3[8];                    // 191
} table_data_t3;

static table_data_t3 g_data_t3 = {
    .dls_inten = 0,
    .dls_rate = 0,
    .dls_pre = 0,
    .dls_samp = 0,
    .dls_post = 0,
    .seq_mode = 0,
    .seq_ctrl = 0,
    .exp_mode = 0,
    .exp_ctrl = 0,
    .pdls_rw1 = {"ld1", "ld2", "ld3", "ld4", "ld5", "ld6", "ld7", "ld8"},
    .pdls_rw2 = {"ld9", "ld10", "ld11", "ld12", "ld13", "ld14", "ld15", "ld16"},
    .pdls_rw3 = {"ld17", "ld18", "ld19", "ld20", "ld21", "ld22", "ld23", "ld24"},
    .pdls_mode = 0,
    .pdls_vl1 = {0, 0, 0, 0, 0, 0, 0, 0},
    .pdls_vl2 = {0, 0, 0, 0, 0, 0, 0, 0},
    .pdls_vl3 = {0, 0, 0, 0, 0, 0, 0, 0},
    .pdls_cr1 = {0, 0, 0, 0, 0, 0, 0, 0},
    .pdls_cr2 = {0, 0, 0, 0, 0, 0, 0, 0},
    .pdls_cr3 = {0, 0, 0, 0, 0, 0, 0, 0},
};

const gs_param_table_row_t data_table3_rows[] = {
    {.addr = PARAM_T3_DLS_INTEN, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "dls_inten"},
    {.addr = PARAM_T3_DLS_RATE, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "dls_rate"},
    {.addr = PARAM_T3_DLS_PRE, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "dls_pre"},
    {.addr = PARAM_T3_DLS_SAMP, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "dls_samp"},
    {.addr = PARAM_T3_DLS_POST, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "dls_post"},
    {.addr = PARAM_T3_SEQ_MODE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "seq_mode"},
    {.addr = PARAM_T3_SEQ_CTRL, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "seq_ctrl"},
    {.addr = PARAM_T3_EXP_MODE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "exp_mode"},
    {.addr = PARAM_T3_EXP_CTRL, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "exp_ctrl"},
    {.addr = PARAM_T3_PDLS_RW1, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 8, .flags = 0, .name = "pdls_rw1"},
    {.addr = PARAM_T3_PDLS_RW2, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 8, .flags = 0, .name = "pdls_rw2"},
    {.addr = PARAM_T3_PDLS_RW3, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 8, .flags = 0, .name = "pdls_rw3"},
    {.addr = PARAM_T3_PDLS_MODE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "pdls_mode"},
    {.addr = PARAM_T3_PDLS_VL1, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 8, .flags = 0, .name = "pdls_vl1"},
    {.addr = PARAM_T3_PDLS_VL2, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 8, .flags = 0, .name = "pdls_vl2"},
    {.addr = PARAM_T3_PDLS_VL3, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 8, .flags = 0, .name = "pdls_vl3"},
    {.addr = PARAM_T3_PDLS_CR1, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 8, .flags = 0, .name = "pdls_cr1"},
    {.addr = PARAM_T3_PDLS_CR2, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 8, .flags = 0, .name = "pdls_cr2"},
    {.addr = PARAM_T3_PDLS_CR3, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 8, .flags = 0, .name = "pdls_cr3"},
};

const size_t data_table3_row_count = sizeof(data_table3_rows) / sizeof(data_table3_rows[0]);

/* ============================================================================
 * Table 4: Telemetry Parameter
 * ============================================================================ */

typedef struct __attribute__((packed)) {
    uint32_t time_now;                      // 0
    uint32_t uptime;                        // 4
    int16_t brd_temp;                      // 8
    uint8_t rst_cause;                     // 10
    uint8_t boot_cause;                    // 11
    uint16_t boot_cnt;                      // 12
    uint8_t dev_type;                      // 14
    char dev_status[8];               // 15
    char ntc_name[8][4];       // 23
    int16_t ntc_value[8];                   // 55
    char sen_name[5][4];       // 71
    int16_t sen_value[5];                   // 91
    char abps_name[2][4];      // 101
    int16_t abps_value[2];                  // 109
    char imu_name[7][4];       // 113
    int16_t imu_value[7];                   // 141
    char v_name[10][4];         // 155
    int16_t v_value[11];                    // 195
    char c_name[10][4];         // 217
    int16_t c_value[10];                    // 257
    char now_mode[16];                 // 277
    uint16_t now_cnter;                     // 293
} table_data_t4;

static table_data_t4 g_data_t4 = {
    .time_now = 0,
    .uptime = 0,
    .brd_temp = 0,
    .rst_cause = 0,
    .boot_cause = 0,
    .boot_cnt = 0,
    .dev_type = 0,
    .dev_status = "OK",
    .ntc_name = {"ntc1", "ntc2", "ntc3", "ntc4", "ntc5", "ntc6", "ntc7", "ntc8"},
    .ntc_value = {-1, -1, -1, -1, -1, -1, -1, -1},
    .sen_name = {"sen1", "sen2", "sen3", "sen4", "sen5"},
    .sen_value = {-1, -1, -1, -1, -1},
    .abps_name = {"prs", "temp"},
    .abps_value = {-1, -1},
    .imu_name = {"ax", "ay", "az", "gx", "gy", "gz", "temp"},
    .imu_value = {-1, -1, -1, -1, -1, -1, -1},
    .v_name = {"if", "hter", "lspd", "som", "hd4", "sln", "tec1", "tec2", "tec3", "tec4"},
    .v_value = {12, 12, 12, 5, 5, 5, 53, -1, -1, -1, -1},
    .c_name = {"if", "hter", "lspd", "som", "hd4", "sln", "tec1", "tec2", "tec3", "tec4"},
    .c_value = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    .now_mode = "IDLE",
    .now_cnter = 0,
};

const gs_param_table_row_t data_table4_rows[] = {
    {.addr = PARAM_T4_TIME_NOW, .type = GS_PARAM_UINT32, .size = sizeof(uint32_t),
     .array_size = 1, .flags = 0, .name = "time_now"},
    {.addr = PARAM_T4_UPTIME, .type = GS_PARAM_UINT32, .size = sizeof(uint32_t),
     .array_size = 1, .flags = 0, .name = "uptime"},
    {.addr = PARAM_T4_BRD_TEMP, .type = GS_PARAM_INT16, .size = sizeof(int16_t),
     .array_size = 1, .flags = 0, .name = "brd_temp"},
    {.addr = PARAM_T4_RST_CAUSE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "rst_cause"},
    {.addr = PARAM_T4_BOOT_CAUSE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "boot_cause"},
    {.addr = PARAM_T4_BOOT_CNT, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "boot_cnt"},
    {.addr = PARAM_T4_DEV_TYPE, .type = GS_PARAM_UINT8, .size = sizeof(uint8_t),
     .array_size = 1, .flags = 0, .name = "dev_type"},
    {.addr = PARAM_T4_DEV_STATUS, .type = GS_PARAM_STRING, .size = 8,
     .array_size = 1, .flags = 0, .name = "dev_status"},
    {.addr = PARAM_T4_NTC_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 8, .flags = 0, .name = "ntc_name"},
    {.addr = PARAM_T4_NTC_VALUE, .type = GS_PARAM_INT16, .size = sizeof(int16_t),
     .array_size = 8, .flags = 0, .name = "ntc_value"},
    {.addr = PARAM_T4_SEN_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 5, .flags = 0, .name = "sen_name"},
    {.addr = PARAM_T4_SEN_VALUE, .type = GS_PARAM_INT16, .size = sizeof(int16_t),
     .array_size = 5, .flags = 0, .name = "sen_value"},
    {.addr = PARAM_T4_ABPS_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 2, .flags = 0, .name = "abps_name"},
    {.addr = PARAM_T4_ABPS_VALUE, .type = GS_PARAM_INT16, .size = sizeof(int16_t),
     .array_size = 2, .flags = 0, .name = "abps_value"},
    {.addr = PARAM_T4_IMU_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 7, .flags = 0, .name = "imu_name"},
    {.addr = PARAM_T4_IMU_VALUE, .type = GS_PARAM_INT16, .size = sizeof(int16_t),
     .array_size = 7, .flags = 0, .name = "imu_value"},
    {.addr = PARAM_T4_V_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 10, .flags = 0, .name = "v_name"},
    {.addr = PARAM_T4_V_VALUE, .type = GS_PARAM_INT16, .size = sizeof(int16_t),
     .array_size = 11, .flags = 0, .name = "v_value"},
    {.addr = PARAM_T4_C_NAME, .type = GS_PARAM_STRING, .size = 4,
     .array_size = 10, .flags = 0, .name = "c_name"},
    {.addr = PARAM_T4_C_VALUE, .type = GS_PARAM_INT16, .size = sizeof(int16_t),
     .array_size = 10, .flags = 0, .name = "c_value"},
    {.addr = PARAM_T4_NOW_MODE, .type = GS_PARAM_STRING, .size = 16,
     .array_size = 1, .flags = 0, .name = "now_mode"},
    {.addr = PARAM_T4_NOW_CNTER, .type = GS_PARAM_UINT16, .size = sizeof(uint16_t),
     .array_size = 1, .flags = 0, .name = "now_cnter"},
};

const size_t data_table4_row_count = sizeof(data_table4_rows) / sizeof(data_table4_rows[0]);

/* ============================================================================
 * Initialization Functions
 * ============================================================================ */

gs_error_t data_table0_init(gs_param_table_instance_t *table) {
    if (!table) return GS_ERROR_ARG;
    
    memset(table, 0, sizeof(*table));
    table->rows = data_table0_rows;
    table->row_count = data_table0_row_count;
    table->memory = &g_data_t0;
    table->memory_size = sizeof(g_data_t0);
    table->flags = 0;
    
    gs_param_table_checksum_le(table);
    
    printf("[TABLE 0] Board Parameter: %u rows, %u bytes, checksum BE=0x%04X\n",
           table->row_count, table->memory_size, table->checksum_be);
    
    return GS_OK;
}

gs_error_t data_table1_init(gs_param_table_instance_t *table) {
    if (!table) return GS_ERROR_ARG;
    
    memset(table, 0, sizeof(*table));
    table->rows = data_table1_rows;
    table->row_count = data_table1_row_count;
    table->memory = &g_data_t1;
    table->memory_size = sizeof(g_data_t1);
    table->flags = 0;
    
    gs_param_table_checksum_le(table);
    
    printf("[TABLE 1] Sys-config Parameter: %u rows, %u bytes, checksum BE=0x%04X\n",
           table->row_count, table->memory_size, table->checksum_be);
    
    return GS_OK;
}

gs_error_t data_table2_init(gs_param_table_instance_t *table) {
    if (!table) return GS_ERROR_ARG;
    
    memset(table, 0, sizeof(*table));
    table->rows = data_table2_rows;
    table->row_count = data_table2_row_count;
    table->memory = &g_data_t2;
    table->memory_size = sizeof(g_data_t2);
    table->flags = 0;
    
    gs_param_table_checksum_le(table);

    printf("[TABLE 2] Manual-Control Parameter: %u rows, %u bytes, checksum BE=0x%04X\n",
           table->row_count, table->memory_size, table->checksum_be);
    
    return GS_OK;
}

gs_error_t data_table3_init(gs_param_table_instance_t *table) {
    if (!table) return GS_ERROR_ARG;
    
    memset(table, 0, sizeof(*table));
    table->rows = data_table3_rows;
    table->row_count = data_table3_row_count;
    table->memory = &g_data_t3;
    table->memory_size = sizeof(g_data_t3);
    table->flags = 0;
    

    gs_param_table_checksum_le(table);
    
    printf("[TABLE 3] Experiment Parameter: %u rows, %u bytes, checksum BE=0x%04X\n",
           table->row_count, table->memory_size, table->checksum_be);
    
    return GS_OK;
}

gs_error_t data_table4_init(gs_param_table_instance_t *table) {
    if (!table) return GS_ERROR_ARG;
    
    memset(table, 0, sizeof(*table));
    table->rows = data_table4_rows;
    table->row_count = data_table4_row_count;
    table->memory = &g_data_t4;
    table->memory_size = sizeof(g_data_t4);
    table->flags = 0;
    
    gs_param_table_checksum_le(table);
    
    printf("[TABLE 4] Telemetry Parameter: %u rows, %u bytes, checksum BE=0x%04X\n",
           table->row_count, table->memory_size, table->checksum_be);
    
    return GS_OK;
}

gs_error_t data_tables_init_all(gs_param_table_instance_t tables[5]) {
    if (!tables) return GS_ERROR_ARG;
    
    printf("Initializing 5 data parameter tables...\n");
    
    gs_error_t err;
    
    err = data_table0_init(&tables[0]);
    if (err != GS_OK) return err;
    
    err = data_table1_init(&tables[1]);
    if (err != GS_OK) return err;
    
    err = data_table2_init(&tables[2]);
    if (err != GS_OK) return err;
    
    err = data_table3_init(&tables[3]);
    if (err != GS_OK) return err;
    
    err = data_table4_init(&tables[4]);
    if (err != GS_OK) return err;
    
    printf("All 5 tables initialized successfully\n\n");
    
    return GS_OK;
}

void data_tables_free_all(gs_param_table_instance_t tables[5]) {
    if (tables) {
        for (int i = 0; i < 5; i++) {
            gs_param_table_free(&tables[i]);
        }
    }
}

const char* data_table_get_name(uint8_t table_id) {
    switch (table_id) {
        case 0: return "Board Parameter";
        case 1: return "Sys-config Parameter";
        case 2: return "Manual-Control Parameter";
        case 3: return "Experiment Parameter";
        case 4: return "Telemetry Parameter";
        default: return "Unknown";
    }
}