#ifndef EXP_RPARAM_TABLE_H
#define EXP_RPARAM_TABLE_H

#include <param/types.h>
#include <param/table.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Table 0: Board Parameter
 * ============================================================================ */
#define PARAM_T0_UID                          0  // 0x00
#define PARAM_T0_TYPE                        16  // 0x10
#define PARAM_T0_REV                         24  // 0x18
#define PARAM_T0_CSP_ADDR                    27  // 0x1B
#define PARAM_T0_CSP_PORT                    28  // 0x1C
#define PARAM_T0_CSP_RTABLE                  44  // 0x2C
#define PARAM_T0_CAN_SPEED                  140  // 0x8C
#define PARAM_T0_REBOOT_IN                  142  // 0x8E
#define PARAM_T0_DEFAULT_IN                 144  // 0x90

extern const gs_param_table_row_t data_table0_rows[];
extern const size_t data_table0_row_count;

/* ============================================================================
 * Table 1: Sys-config Parameter
 * ============================================================================ */
#define PARAM_T1_TIME_SET                     0  // 0x00
#define PARAM_T1_POWER_NAME                   4  // 0x04
#define PARAM_T1_POWER_SET                   32  // 0x20
#define PARAM_T1_I2C_NAME                    39  // 0x27
#define PARAM_T1_I2C_SET                     63  // 0x3F
#define PARAM_T1_LED_EN                      69  // 0x45
#define PARAM_T1_IFB_EN                      70  // 0x46
#define PARAM_T1_MPU_EN                      71  // 0x47
#define PARAM_T1_OTA_STATUS                  72  // 0x48
#define PARAM_T1_OTA_CTRL                    73  // 0x49

extern const gs_param_table_row_t data_table1_rows[];
extern const size_t data_table1_row_count;

/* ============================================================================
 * Table 2: Manual-Control Parameter
 * ============================================================================ */
#define PARAM_T2_SIG_MPU                      0  // 0x00
#define PARAM_T2_SLN_NAME                     1  // 0x01
#define PARAM_T2_SLN_MODE                    17  // 0x11
#define PARAM_T2_SLN_CTRL                    18  // 0x12
#define PARAM_T2_SL_GR1                      22  // 0x16
#define PARAM_T2_SL_GR2                      54  // 0x36
#define PARAM_T2_SL_MODE                     86  // 0x56
#define PARAM_T2_SL_CTRL1                    87  // 0x57
#define PARAM_T2_SL_CTRL2                    95  // 0x5F
#define PARAM_T2_HD4_NAME                   103  // 0x67
#define PARAM_T2_HD4_MODE                   135  // 0x87
#define PARAM_T2_HD4_CTRL                   136  // 0x88
#define PARAM_T2_VAL_CFG                    144  // 0x90
#define PARAM_T2_VAL_CTRL                   145  // 0x91
#define PARAM_T2_TEC_NAME                   146  // 0x92
#define PARAM_T2_TEC_MODE                   162  // 0xA2
#define PARAM_T2_TEC_TEMP                   163  // 0xA3
#define PARAM_T2_HTER_NAME                  171  // 0xAB
#define PARAM_T2_HTER_MODE                  203  // 0xCB
#define PARAM_T2_HTER_TEMP                  204  // 0xCC
#define PARAM_T2_CTRL_STATUS                220  // 0xDC

extern const gs_param_table_row_t data_table2_rows[];
extern const size_t data_table2_row_count;

/* ============================================================================
 * Table 3: Experiment Parameter
 * ============================================================================ */
#define PARAM_T3_DLS_INTEN                    0  // 0x00
#define PARAM_T3_DLS_RATE                     2  // 0x02
#define PARAM_T3_DLS_PRE                      4  // 0x04
#define PARAM_T3_DLS_SAMP                     6  // 0x06
#define PARAM_T3_DLS_POST                     8  // 0x08
#define PARAM_T3_SEQ_MODE                    10  // 0x0A
#define PARAM_T3_SEQ_CTRL                    11  // 0x0B
#define PARAM_T3_EXP_MODE                    12  // 0x0C
#define PARAM_T3_EXP_CTRL                    13  // 0x0D
#define PARAM_T3_PDLS_RW1                    14  // 0x0E
#define PARAM_T3_PDLS_RW2                    46  // 0x2E
#define PARAM_T3_PDLS_RW3                    78  // 0x4E
#define PARAM_T3_PDLS_MODE                  110  // 0x6E
#define PARAM_T3_PDLS_VL1                   111  // 0x6F
#define PARAM_T3_PDLS_VL2                   127  // 0x7F
#define PARAM_T3_PDLS_VL3                   143  // 0x8F
#define PARAM_T3_PDLS_CR1                   159  // 0x9F
#define PARAM_T3_PDLS_CR2                   175  // 0xAF
#define PARAM_T3_PDLS_CR3                   191  // 0xBF

extern const gs_param_table_row_t data_table3_rows[];
extern const size_t data_table3_row_count;

/* ============================================================================
 * Table 4: Telemetry Parameter
 * ============================================================================ */
#define PARAM_T4_TIME_NOW                     0  // 0x00
#define PARAM_T4_UPTIME                       4  // 0x04
#define PARAM_T4_BRD_TEMP                     8  // 0x08
#define PARAM_T4_RST_CAUSE                   10  // 0x0A
#define PARAM_T4_BOOT_CAUSE                  11  // 0x0B
#define PARAM_T4_BOOT_CNT                    12  // 0x0C
#define PARAM_T4_DEV_TYPE                    14  // 0x0E
#define PARAM_T4_DEV_STATUS                  15  // 0x0F
#define PARAM_T4_NTC_NAME                    23  // 0x17
#define PARAM_T4_NTC_VALUE                   55  // 0x37
#define PARAM_T4_SEN_NAME                    71  // 0x47
#define PARAM_T4_SEN_VALUE                   91  // 0x5B
#define PARAM_T4_ABPS_NAME                  101  // 0x65
#define PARAM_T4_ABPS_VALUE                 109  // 0x6D
#define PARAM_T4_IMU_NAME                   113  // 0x71
#define PARAM_T4_IMU_VALUE                  141  // 0x8D
#define PARAM_T4_V_NAME                     155  // 0x9B
#define PARAM_T4_V_VALUE                    195  // 0xC3
#define PARAM_T4_C_NAME                     217  // 0xD9
#define PARAM_T4_C_VALUE                    257  // 0x101
#define PARAM_T4_NOW_MODE                   277  // 0x115
#define PARAM_T4_NOW_CNTER                  293  // 0x125

extern const gs_param_table_row_t data_table4_rows[];
extern const size_t data_table4_row_count;

/* ============================================================================
 * Initialization Functions
 * ============================================================================ */

/**
 * Initialize all 5 tables
 * @param tables Array of 5 table instances
 * @return GS_OK on success
 */
gs_error_t data_tables_init_all(gs_param_table_instance_t *tables);
void data_tables_free_all(gs_param_table_instance_t *tables);

/**
 * Initialize individual tables
 */
gs_error_t data_table0_init(gs_param_table_instance_t *table);
gs_error_t data_table1_init(gs_param_table_instance_t *table);
gs_error_t data_table2_init(gs_param_table_instance_t *table);
gs_error_t data_table3_init(gs_param_table_instance_t *table);
gs_error_t data_table4_init(gs_param_table_instance_t *table);
/**
 * Get table name for display
 */
const char* data_table_get_name(uint8_t table_id);

#ifdef __cplusplus
}
#endif

#endif // EXP_RPARAM_TABLE_H