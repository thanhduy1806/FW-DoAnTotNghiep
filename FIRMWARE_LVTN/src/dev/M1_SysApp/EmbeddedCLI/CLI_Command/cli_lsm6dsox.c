#include "cli_lsm6dsox.h"
#include "stdio.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "stdlib.h"
#include "string.h"

#include "M2_BSP/BSP_LSM6DSOX/bsp_lsm6dsox.h"

extern lsm6d_dev_t  lsm6dsox;
extern i2c_io_t i2c1;
extern lsm6d_data_t lsm6d_all_data;
extern Accel_Gyro_t lsm6d_accel_data;
extern Accel_Gyro_t lsm6d_gyro_data;
extern float        lsm6d_temperature;
/*************************************************
 *             Command List Function             *
 *************************************************/
void CMD_LSM6DSOX_Config_Gyro (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    
    uint16_t gy_fs_table[5] = {125, 250, 500, 1000, 2000};
    uint16_t gy_odr_table[8] = {0, 12, 26, 52, 104, 208, 416, 833};
    
    const char *odrStr = embeddedCliGetToken(args, 1);
    const char *fsStr = embeddedCliGetToken(args, 2);
    
    if (odrStr == NULL || fsStr == NULL) {
        embeddedCliPrint(cli, "Usage: lsm6d_conf_gyro <odr> <fs>");
        return;
    }

    uint8_t odr = (uint8_t) strtoul(odrStr, NULL, 0);
    uint8_t fs = (uint8_t) strtoul(fsStr, NULL, 0);
    
    if(odr < 0 || odr > 7){
        embeddedCliPrint(cli, "Invalid odr (must be 0-7). Please enter again.");
        return;
    }
    
    if(fs < 0 || fs > 4){
        embeddedCliPrint(cli, "Invalid fs (must be 0-4). Please enter again.");
        return;
    }
    
    if(bsp_lsm6dsox_conf_gyro(odr,fs) == ERROR_OK)
    {
        snprintf(buf, sizeof (buf),"Configure Gyroscope:%d Hz, %d dps", gy_odr_table[odr], gy_fs_table[fs]);
        embeddedCliPrint(cli, buf);
    }
}

void CMD_LSM6DSOX_Config_Accel (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    
    uint8_t xl_fs_table[4] = {2, 16, 4, 8};
    uint16_t xl_odr_table[8] = {0, 12, 26, 52, 104, 208, 416, 833};
    
    const char *odrStr = embeddedCliGetToken(args, 1);
    const char *fsStr = embeddedCliGetToken(args, 2);
    
    if (odrStr == NULL || fsStr == NULL) {
        embeddedCliPrint(cli, "Usage: lsm6d_conf_accel <odr> <fs>");
        return;
    }

    uint8_t odr = (uint8_t) strtoul(odrStr, NULL, 0);
    uint8_t fs = (uint8_t) strtoul(fsStr, NULL, 0);
    
    if(odr < 0 || odr > 7){
        embeddedCliPrint(cli, "Invalid odr (must be 0-7). Please enter again.");
        return;
    }
    
    if(fs < 0 || fs > 3){
        embeddedCliPrint(cli, "Invalid fs (must be 0-3). Please enter again.");
        return;
    }
    
    if(bsp_lsm6dsox_conf_accel(odr,fs) == ERROR_OK)
    {
        snprintf(buf, sizeof (buf),"Configure Accelerometer: %d Hz, %d g", xl_odr_table[odr], xl_fs_table[fs]);
        embeddedCliPrint(cli, buf);
    }
}

void CMD_LSM6DSOX_Config_Int (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    
    static const char* INT1_MODE_NAMES[] = {
        "NONE",              // 0
        "Accel Data Ready",  // 1
        "Gyro Data Ready",   // 2
        "Temp Data Ready",   // 3
        "FIFO Threshold",    // 4
        "FIFO Overrun",      // 5
        "FIFO Full",         // 6
        "Counter BDR"        // 7
    };

    static const char* INT1_EVENT_NAMES[] = {
        "NO Event",          // 0
        "Sensor Hub",        // 1
        "Embedded Func",     // 2
        "6D Orientation",    // 3
        "Double Tap",        // 4
        "Free-Fall",         // 5
        "Wake-Up",           // 6
        "Single Tap",        // 7
        "Sleep Change"       // 8
    };
    
    const char *modeStr = embeddedCliGetToken(args, 1);
    const char *eventStr = embeddedCliGetToken(args, 2);
    
    if (modeStr == NULL || eventStr == NULL) {
        embeddedCliPrint(cli, "Usage: lsm6d_conf_int <mode> <event>");
        return;
    }

    uint8_t mode = (uint8_t) strtoul(modeStr, NULL, 0);
    uint8_t event = (uint8_t) strtoul(eventStr, NULL, 0);
    
    if(mode < 0 || mode > 7){
        embeddedCliPrint(cli, "Invalid mode (must be 0-6). Please enter again.");
        return;
    }
    
    if(event < 0 || event > 8){
        embeddedCliPrint(cli, "Invalid event (must be 0-8). Please enter again.");
        return;
    }
    
    if(bsp_lsm6dsox_conf_int(mode, event) == ERROR_OK)
    {
        snprintf(buf, sizeof(buf), "Success! INT1 Configured:");
        embeddedCliPrint(cli, buf);
        
        snprintf(buf, sizeof(buf), "Mode: %s", INT1_MODE_NAMES[mode]);
        embeddedCliPrint(cli, buf);      
        
        snprintf(buf, sizeof(buf), "Event: %s", INT1_EVENT_NAMES[event]);
        embeddedCliPrint(cli, buf);
    }
}

void CMD_LSM6DSOX_Dis_Int (EmbeddedCli *cli, char *args, void *context)
{
    if(bsp_lsm6dsox_disable_int() == ERROR_OK)
    {
        embeddedCliPrint(cli, "Disable Interrupt");
    }
}

void CMD_LSM6DSOX_Config_FF (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    uint16_t ths_val[] = {156, 219, 250, 312, 344, 406, 469, 500};
    
    const char *thsStr = embeddedCliGetToken(args, 1);
    const char *durStr = embeddedCliGetToken(args, 2);
    
    if (thsStr == NULL || durStr == NULL) {
        embeddedCliPrint(cli, "Usage: lsm6d_conf_ff <ths> <dur>");
        return;
    }

    uint8_t ths = (uint8_t) strtoul(thsStr, NULL, 0);
    uint8_t dur = (uint8_t) strtoul(durStr, NULL, 0);
    
    if(ths < 0 || ths > 7){
        embeddedCliPrint(cli, "Invalid Threshold (must be 0-7). Please enter again.");
        return;
    }
    
    if(dur < 0 || dur > 63){
        embeddedCliPrint(cli, "Invalid Duration (must be 0-63). Please enter again.");
        return;
    }
    
    if(bsp_lsm6dsox_conf_ff(ths, dur) == ERROR_OK)
    {
        snprintf(buf, sizeof(buf), "Success! Free-fall Configured:");
        embeddedCliPrint(cli, buf);
        
        snprintf(buf, sizeof(buf), "Threshold: %d mg", ths_val[ths]);
        embeddedCliPrint(cli, buf);      
        
        snprintf(buf, sizeof(buf), "Duration: %d x ODR_time", dur);
        embeddedCliPrint(cli, buf);
    }
}

void CMD_LSM6DSOX_Config_WU (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    
    const char *thsStr = embeddedCliGetToken(args, 1);
    const char *durStr = embeddedCliGetToken(args, 2);
    
    if (thsStr == NULL || durStr == NULL) {
        embeddedCliPrint(cli, "Usage: lsm6d_conf_wu <ths> <dur>");
        return;
    }

    uint8_t ths = (uint8_t) strtoul(thsStr, NULL, 0);
    uint8_t dur = (uint8_t) strtoul(durStr, NULL, 0);
    
    if(ths < 0 || ths > 63){
        embeddedCliPrint(cli, "Invalid Threshold (must be 0-63). Please enter again.");
        return;
    }
    
    if(dur < 0 || dur > 3){
        embeddedCliPrint(cli, "Invalid Duration (must be 0-3). Please enter again.");
        return;
    }
    
    if(bsp_lsm6dsox_conf_wu(ths, dur) == ERROR_OK)
    {
        snprintf(buf, sizeof(buf), "Success! Wake-up Configured:");
        embeddedCliPrint(cli, buf);
        
        snprintf(buf, sizeof(buf), "Threshold: %d x FS_XL/2^6", ths);
        embeddedCliPrint(cli, buf);      
        
        snprintf(buf, sizeof(buf), "Duration: %d x ODR_time", dur);
        embeddedCliPrint(cli, buf);
    }
}

void CMD_LSM6DSOX_Init (EmbeddedCli *cli, char *args, void *context)
{
    if(bsp_lsm6dsox_init() == ERROR_OK)
    {
        embeddedCliPrint(cli, "Init Success"); 
    }
}

void CMD_LSM6DSOX_Read_Accel (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    if(bsp_lsm6dsox_read_accel() != ERROR_OK)
    {
        embeddedCliPrint(cli, "LSM6DSOX: Read Accel Failed. Please init first");
        return;
    }
    
    snprintf(buf, sizeof(buf), "Accel : X=%.2f mg   Y=%.2f mg   Z=%.2f mg",
             lsm6d_accel_data.x,
             lsm6d_accel_data.y,
             lsm6d_accel_data.z);
    embeddedCliPrint(cli, buf);
}

void CMD_LSM6DSOX_Read_Gyro (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    if(bsp_lsm6dsox_read_gyro() != ERROR_OK)
    {
        embeddedCliPrint(cli, "LSM6DSOX: Read Gyro Failed. Please init first");
        return;
    }
    
    snprintf(buf, sizeof(buf), "Gyro  : X=%.2f dps   Y=%.2f dps    Z=%.2f dps",
             lsm6d_gyro_data.x,
             lsm6d_gyro_data.y,
             lsm6d_gyro_data.z);
    embeddedCliPrint(cli, buf);
}

void CMD_LSM6DSOX_Read_Temp (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    if(bsp_lsm6dsox_read_temp() == ERROR_NOT_READY)
    {
        embeddedCliPrint(cli, "LSM6DSOX: Read Temperature Failed. Please init first");
        return;
    }
    snprintf(buf, sizeof(buf), "Temperature: %.2f C", lsm6d_temperature);
    
    embeddedCliPrint(cli, buf);
}

void CMD_LSM6DSOX_Read_All (EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    if(bsp_lsm6dsox_read_all() != ERROR_OK)
    {
        embeddedCliPrint(cli, "LSM6DSOX: Read Fail. Please init first");
        return;
    }
    
    snprintf(buf, sizeof(buf), "Gyro  : X=%.2f dps   Y=%.2f dps    Z=%.2f dps",
             lsm6d_all_data.Gyro.x,
             lsm6d_all_data.Gyro.y,
             lsm6d_all_data.Gyro.z);
    embeddedCliPrint(cli, buf);

    snprintf(buf, sizeof(buf), "Accel : X=%.2f mg   Y=%.2f mg   Z=%.2f mg",
             lsm6d_all_data.Accel.x,
             lsm6d_all_data.Accel.y,
             lsm6d_all_data.Accel.z);
    embeddedCliPrint(cli, buf);
    
    snprintf(buf, sizeof(buf), "Temp : %.2f C",
             lsm6d_all_data.Temperature);
    embeddedCliPrint(cli, buf);
}

void CMD_LSM6DSOX_Check (EmbeddedCli *cli, char *args, void *context)
{
    Std_ReturnType r;
    r = bsp_lsm6dsox_check();
    if(r != ERROR_OK)
    {
        embeddedCliPrint(cli, "LSM6DSOX Check Fail");
        return;
    }
    
    embeddedCliPrint(cli, "LSM6DSOX Check OK");
}