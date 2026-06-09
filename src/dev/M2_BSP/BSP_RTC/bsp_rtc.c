#include "bsp_rtc.h"

extern i2c_io_t i2c1;

rv3129_t bsp_rtc_t = {
    .bus = &i2c1,
    .addr7 = RV3129_ADDR
};

void bsp_rtc_init( void )
{
    rv3129_init(&bsp_rtc_t, &i2c1, RV3129_ADDR);
    uint8_t dummy;
    rv3129_read_reg(&bsp_rtc_t, RV3129_CTRL_STATUS, &dummy);
    rv3129_set_24h(&bsp_rtc_t);
}

void bsp_rtc_get(bsp_rtc_datetime_t* dt)
{
    uint8_t verify;
    rv3129_read_reg(&bsp_rtc_t, RV3129_CTRL_1, &verify);
    if((verify & 1U) == 0)
    {
    rv3129_write_reg(&bsp_rtc_t, RV3129_CTRL_1, 0x99);
    }
    
    rv3129_update_time(&bsp_rtc_t);
    
    dt->sec     = rv3129_get_seconds(&bsp_rtc_t);
    dt->min     = rv3129_get_minutes(&bsp_rtc_t);
    dt->hour    = rv3129_get_hours(&bsp_rtc_t);
    dt->date    = rv3129_get_date(&bsp_rtc_t);
    dt->month   = rv3129_get_month(&bsp_rtc_t);
    dt->year    = (uint16_t)(2000u + rv3129_get_year(&bsp_rtc_t));
    dt->weekday = rv3129_get_weekday(&bsp_rtc_t);
    dt->is_pm   = false;
}

void bsp_rtc_set(const bsp_rtc_datetime_t* dt)
{
    rv3129_set_time(&bsp_rtc_t,
                            dt->sec, dt->min, dt->hour,
                            dt->date, dt->month, dt->year,
                            dt->weekday);
}

void bsp_rtc_set_sec(uint8_t sec)
{
    rv3129_set_seconds(&bsp_rtc_t, sec);
}

void bsp_rtc_set_min(uint8_t min)
{
    rv3129_set_minutes(&bsp_rtc_t, min);
}

void bsp_rtc_set_hour(uint8_t hour)
{
    rv3129_set_hours(&bsp_rtc_t, hour);
}

void bsp_rtc_set_date(uint8_t date)
{
    rv3129_set_date(&bsp_rtc_t, date);
}

void bsp_rtc_set_month(uint8_t month)
{
    rv3129_set_month(&bsp_rtc_t, month);
}

void bsp_rtc_set_weekday(uint8_t weekday)
{
    rv3129_set_weekday(&bsp_rtc_t, weekday);
}

void bsp_rtc_set_year(uint8_t year)
{
    rv3129_set_year(&bsp_rtc_t, year);
}

/* =========================================================
 * Temperature
 * ========================================================= */ 
void bsp_rtc_get_temperature(int8_t* temp_c)
{
    rv3129_get_temp(&bsp_rtc_t, temp_c);
}
 
/* =========================================================
 * Alarm
 * ========================================================= */
void bsp_rtc_alarm_set(const bsp_rtc_alarm_cfg_t* cfg)
{
    rv3129_set_alarm_hms_dmYwd(&bsp_rtc_t, cfg->sec, cfg->min,
                                cfg->hour, cfg->date, cfg->weekday,
                                cfg->month, cfg->year2d);
    
    uint8_t enableBits = (1u<<2)|(1u<<1)|(1u<<0);
            
    rv3129_alarm_enable_mask(&bsp_rtc_t, enableBits);
    rv3129_alarm_int_enable(&bsp_rtc_t, false);
}

void bsp_rtc_alarm_disable( void )
{
    rv3129_alarm_enable_mask(&bsp_rtc_t, 0x00U);
    rv3129_alarm_int_enable(&bsp_rtc_t, false);
}

void bsp_rtc_alarm_flag_get(bool* triggered)
{
    rv3129_alarm_flag_get(&bsp_rtc_t, triggered);
}

void bsp_rtc_alarm_flag_clear( void )
{
    uint8_t v;
    rv3129_read_reg(&bsp_rtc_t, RV3129_CTRL_INT_FLAG, &v);
    v &= (uint8_t)~0x01u;
    rv3129_write_reg(&bsp_rtc_t, RV3129_CTRL_INT_FLAG, v);
}

/* =========================================================
 * Timer
 * ========================================================= */
void bsp_rtc_timer_set(uint16_t ticks)
{
    rv3129_timer_set(&bsp_rtc_t, ticks);
}

void bsp_rtc_timer_int_enable(void)
{
    rv3129_timer_int_enable(&bsp_rtc_t, true);
}

/* =========================================================
 * Reset
 * ========================================================= */
void bsp_rtc_reset( void )
{
    rv3129_system_reset(&bsp_rtc_t);
}

uint8_t bsp_rtc_ping( void )
{
    uint8_t r;
    rv3129_write_reg(&bsp_rtc_t, RV3129_USER_RAM, 0x30);
    rv3129_read_reg(&bsp_rtc_t, RV3129_USER_RAM, &r);
    if(r == 0x30)
    {
        return true;
    }
    
    return false;
}