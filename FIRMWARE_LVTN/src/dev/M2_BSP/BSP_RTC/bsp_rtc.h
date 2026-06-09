#ifndef BSP_RTC_H
#define	BSP_RTC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "M3_Driver/devices/RV3129/rv3129.h"
    
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct {
    uint8_t  sec;       /**< 0..59 */
    uint8_t  min;       /**< 0..59 */
    uint8_t  hour;      /**< 0..23 (24h) ho?c 1..12 (12h) */
    uint8_t  date;      /**< 1..31 */
    uint8_t  month;     /**< 1..12 */
    uint16_t year;      /**< 2000..2099 */
    uint8_t  weekday;   /**< 1=Mon .. 7=Sun (ISO 8601), ho?c 1..7 theo config */
    bool     is_pm;     /**< ch? dùng khi ? ch? ?? 12h */
} bsp_rtc_datetime_t;

typedef struct {
    uint8_t  sec;
    uint8_t  min;
    uint8_t  hour;
    uint8_t  date;
    uint8_t  weekday;
    uint8_t  month;
    uint8_t  year2d;

    uint8_t  enable_mask;
    bool     int_enable;
} bsp_rtc_alarm_cfg_t;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void bsp_rtc_init( void );
void bsp_rtc_get(bsp_rtc_datetime_t* dt);

void bsp_rtc_set(const bsp_rtc_datetime_t* dt);
void bsp_rtc_set_sec(uint8_t sec);
void bsp_rtc_set_min(uint8_t min);
void bsp_rtc_set_hour(uint8_t hour);
void bsp_rtc_set_date(uint8_t date);
void bsp_rtc_set_month(uint8_t month);
void bsp_rtc_set_weekday(uint8_t weekday);
void bsp_rtc_set_year(uint8_t year);

void bsp_rtc_get_temperature(int8_t* temp_c);
void bsp_rtc_alarm_set(const bsp_rtc_alarm_cfg_t* cfg);
void bsp_rtc_alarm_disable( void );
void bsp_rtc_alarm_flag_get(bool* triggered);
void bsp_rtc_alarm_flag_clear( void );
void bsp_rtc_timer_set(uint16_t ticks);
void bsp_rtc_timer_int_enable(void);

void bsp_rtc_reset( void );
uint8_t bsp_rtc_ping( void );

#ifdef	__cplusplus
}
#endif

#endif	/* BSP_RTC_H */
