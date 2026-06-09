#include "cli_rtc.h"
#include "stdio.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "stdlib.h"
#include "string.h"
#include "os.h"


/* =============================================================================
 * if the time not update. Please check the CTRl1 register and set bit0(WE) to 1
 * ========================================================================== */ 

extern rv3129_t bsp_rtc_t;

static const char* const weekday_str[7] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

void CMD_RTC_Read(EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    bsp_rtc_datetime_t dt;
    
    bsp_rtc_get(&dt);
    
    snprintf(buf, sizeof(buf), "TIME: %u:%u:%u", dt.hour, dt.min, dt.sec);
    embeddedCliPrint(cli, buf);
    
    snprintf(buf, sizeof(buf), "%s, %u/%u/%u", weekday_str[(dt.weekday)-1], dt.date, dt.month, dt.year);
    embeddedCliPrint(cli, buf);
}

void CMD_RTC_Set_Date(EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    
    const char *arg4 = embeddedCliGetToken(args, 1); // date
    const char *arg5 = embeddedCliGetToken(args, 2); // month
    const char *arg6 = embeddedCliGetToken(args, 3); // year
    const char *arg7 = embeddedCliGetToken(args, 4); // weekday
    
    if (arg4 == NULL || arg5 == NULL || arg6 == NULL || arg7 == NULL) {
        snprintf(buf, sizeof (buf),
                "Usage: rtc_set_date <day> <month> <year> <weekday>");
        embeddedCliPrint(cli, buf);
        return;
    }
    
    
    uint8_t date    =  (uint8_t)strtoul(arg4, NULL, 0);
    uint8_t month   =  (uint8_t)strtoul(arg5, NULL, 0);
    uint16_t year   =  (uint16_t)strtoul(arg6,NULL, 0);
    uint8_t weekday =  (uint8_t)strtoul(arg7, NULL, 0);
    
    if (date < 1 || date > 31) {
        embeddedCliPrint(cli, "Invalid day (must be 1-31). Please enter again.");
        return;
    }
    if (month < 1 || month > 12) {
        embeddedCliPrint(cli, "Invalid month (must be 1-12). Please enter again.");
        return;
    }
    if (weekday < 1 || weekday > 7) {
        embeddedCliPrint(cli, "Invalid weekday (must be 1-7, 1 for Mon). Please enter again.");
        return;
    }
    
    bsp_rtc_datetime_t dt;
    bsp_rtc_get(&dt);
    
    dt.date = date;
    dt.month = month;
    dt.year = year;
    dt.weekday = weekday;
    
    bsp_rtc_set(&dt);
    
    embeddedCliPrint(cli, "RTC date set OK");
}

void CMD_RTC_Set_Time(EmbeddedCli *cli, char *args, void *context)
{   
    char buf[128];
    
    const char *arg1 = embeddedCliGetToken(args, 1); // hour
    const char *arg2 = embeddedCliGetToken(args, 2); // minute
    const char *arg3 = embeddedCliGetToken(args, 3); // second
    
    if (arg1 == NULL || arg2 == NULL || arg3 == NULL) {
        snprintf(buf, sizeof (buf),
                "Usage: rtc_set_time <hour> <minute> <second>");
        embeddedCliPrint(cli, buf);
        return;
    }
    
    uint8_t hour    =  (uint8_t)strtoul(arg1, NULL, 0);
    uint8_t minute  =  (uint8_t)strtoul(arg2, NULL, 0);
    uint8_t second  =  (uint8_t)strtoul(arg3, NULL, 0);
    
    if (hour < 0 || hour > 23) {
        embeddedCliPrint(cli, "Invalid hour (must be 0-23). Please enter again.");
        return;
    }
    if (minute < 0 || minute > 59) {
        embeddedCliPrint(cli, "Invalid minute (must be 0-59). Please enter again.");
        return;
    }
    if (second < 0 || second > 59) {
        embeddedCliPrint(cli, "Invalid second (must be 0-59). Please enter again.");
        return;
    }
    
    bsp_rtc_datetime_t dt;
    bsp_rtc_get(&dt);
    
    dt.hour = hour;
    dt.min = minute;
    dt.sec = second;
    
    bsp_rtc_set(&dt);
    
    embeddedCliPrint(cli, "RTC time set OK");
}

void CMD_RTC_Ping(EmbeddedCli *cli, char *args, void *context)
{
    uint8_t rc = bsp_rtc_ping();
    if(rc)
    {
       embeddedCliPrint(cli, "RTC OK"); 
    } else {
        embeddedCliPrint(cli, "RTC FAIL"); 
    }
}

void CMD_RTC_Test(EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    bsp_rtc_datetime_t dt1, dt2;

    bsp_rtc_get(&dt1);
    snprintf(buf, sizeof(buf), "T1: %02u:%02u:%02u", dt1.hour, dt1.min, dt1.sec);
    embeddedCliPrint(cli, buf);

    osDelay(pdMS_TO_TICKS(1000));

    bsp_rtc_get(&dt2);
    snprintf(buf, sizeof(buf), "T2: %02u:%02u:%02u", dt2.hour, dt2.min, dt2.sec);
    embeddedCliPrint(cli, buf);

    int16_t diff = (int16_t)dt2.sec - (int16_t)dt1.sec;

    if (diff >= 1)
        embeddedCliPrint(cli, "RTC OK");
    else
        embeddedCliPrint(cli, "RTC FAIL");
}