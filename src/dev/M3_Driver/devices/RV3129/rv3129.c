#include "rv3129.h"
#include <string.h>
#include "define.h"

/* ==== I2C helpers ==== */
static int _wr(rv3129_t* d, const uint8_t* b, uint8_t n) {
    int w = (int)i2c_io_send(d->bus, d->addr7, (const char*)b, n);
    return (w == 0) ? (int)ERROR_OK : (int)ERROR_I2C_TIMEOUT;
}
static int _rd(rv3129_t* d, uint8_t* b, uint8_t n) {
    int r = (int)i2c_io_recv(d->bus, d->addr7, (char*)b, n);
    return (r == 0) ? (int)ERROR_OK : (int)ERROR_I2C_TIMEOUT;
}

int rv3129_init(rv3129_t* dev, i2c_io_t* bus, uint8_t addr7)
{
    if (!dev || !bus) return (int)ERROR_INVALID_PARAM;
    dev->bus   = bus;
    dev->addr7 = (addr7 & 0x7F);
    memset(dev->time, 0, sizeof(dev->time));
    return (int)ERROR_OK;
}

int rv3129_read_regs(rv3129_t* dev, uint8_t reg, uint8_t* dst, uint8_t len)
{
    if (!dev || !dst || !len) return (int)ERROR_INVALID_PARAM;
    int st;
    st = _wr(dev, &reg, 1);
    if (st != (int)ERROR_OK) return st;
    return _rd(dev, dst, len);
}

int rv3129_write_regs(rv3129_t* dev, uint8_t reg, const uint8_t* src, uint8_t len)
{
    if (!dev || !src || !len) return (int)ERROR_INVALID_PARAM;
    uint8_t buf[1 + 16];
    if (len > 16) return (int)ERROR_INVALID_PARAM;
    buf[0] = reg;
    for (uint8_t i=0;i<len;i++) buf[1+i] = src[i];
    return _wr(dev, buf, (uint8_t)(1 + len));
}

int rv3129_read_reg(rv3129_t* dev, uint8_t reg, uint8_t* val)
{
    if (!dev || !val) return (int)ERROR_INVALID_PARAM;
    return rv3129_read_regs(dev, reg, val, 1);
}

int rv3129_write_reg(rv3129_t* dev, uint8_t reg, uint8_t val)
{
    return rv3129_write_regs(dev, reg, &val, 1);
}

/* ==== BCD helpers ==== */
uint8_t rv3129_bcd_to_dec(uint8_t v) { return (uint8_t)((v/16u)*10u + (v%16u)); }
uint8_t rv3129_dec_to_bcd(uint8_t v) { return (uint8_t)(((v/10u)<<4) | (v%10u)); }

/* ==== Time ==== */
int rv3129_update_time(rv3129_t* dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    int st = rv3129_read_regs(dev, RV3129_SECONDS, dev->time, RV3129_TIME_ARRAY_LEN);
    if (st != (int)ERROR_OK) return st;

    bool is12=false;
    (void)rv3129_is_12h(dev, &is12);
    if (is12) dev->time[RV3129_TIME_HOURS] &= (uint8_t)~(1u<<HOURS_AM_PM);
    return (int)ERROR_OK;
}

uint8_t rv3129_get_seconds(const rv3129_t* d){ return rv3129_bcd_to_dec(d->time[RV3129_TIME_SECONDS]); }
uint8_t rv3129_get_minutes(const rv3129_t* d){ return rv3129_bcd_to_dec(d->time[RV3129_TIME_MINUTES]); }
uint8_t rv3129_get_hours  (const rv3129_t* d){ return rv3129_bcd_to_dec(d->time[RV3129_TIME_HOURS]); }
uint8_t rv3129_get_weekday(const rv3129_t* d){ return rv3129_bcd_to_dec(d->time[RV3129_TIME_DAY]); }
uint8_t rv3129_get_date   (const rv3129_t* d){ return rv3129_bcd_to_dec(d->time[RV3129_TIME_DATE]); }
uint8_t rv3129_get_month  (const rv3129_t* d){ return rv3129_bcd_to_dec(d->time[RV3129_TIME_MONTH]); }
uint8_t rv3129_get_year   (const rv3129_t* d){ return rv3129_bcd_to_dec(d->time[RV3129_TIME_YEAR]); }

static int _write_time_block(rv3129_t* dev, uint8_t* blk)
{
    return rv3129_write_regs(dev, RV3129_SECONDS, blk, RV3129_TIME_ARRAY_LEN);
}

int rv3129_set_time(rv3129_t* dev,
                    uint8_t sec, uint8_t min, uint8_t hour,
                    uint8_t date, uint8_t month, uint16_t year, uint8_t weekday)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    uint8_t t[RV3129_TIME_ARRAY_LEN];
    t[RV3129_TIME_SECONDS] = rv3129_dec_to_bcd(sec);
    t[RV3129_TIME_MINUTES] = rv3129_dec_to_bcd(min);
    t[RV3129_TIME_HOURS]   = rv3129_dec_to_bcd(hour);
    t[RV3129_TIME_DATE]    = rv3129_dec_to_bcd(date);
    t[RV3129_TIME_MONTH]   = rv3129_dec_to_bcd(month);
    t[RV3129_TIME_YEAR]    = rv3129_dec_to_bcd((uint8_t)(year >= 2000 ? (year-2000) : year));
    t[RV3129_TIME_DAY]     = rv3129_dec_to_bcd(weekday);

    int st = _write_time_block(dev, t);
    if (st == (int)ERROR_OK) memcpy(dev->time, t, sizeof(t));
    return st;
}

static int _set_field_and_write(rv3129_t* dev, rv3129_time_idx_t idx, uint8_t v_dec)
{
    int st = rv3129_update_time(dev);
    if (st != (int)ERROR_OK) return st;
    dev->time[idx] = rv3129_dec_to_bcd(v_dec);
    return _write_time_block(dev, dev->time);
}
int rv3129_set_seconds(rv3129_t* dev, uint8_t sec)   { return _set_field_and_write(dev, RV3129_TIME_SECONDS, sec); }
int rv3129_set_minutes(rv3129_t* dev, uint8_t min)   { return _set_field_and_write(dev, RV3129_TIME_MINUTES, min); }
int rv3129_set_hours  (rv3129_t* dev, uint8_t hour)  { return _set_field_and_write(dev, RV3129_TIME_HOURS,   hour); }
int rv3129_set_date   (rv3129_t* dev, uint8_t date)  { return _set_field_and_write(dev, RV3129_TIME_DATE,    date); }
int rv3129_set_month  (rv3129_t* dev, uint8_t month) { return _set_field_and_write(dev, RV3129_TIME_MONTH,   month); }
int rv3129_set_year   (rv3129_t* dev, uint8_t y2d)   { return _set_field_and_write(dev, RV3129_TIME_YEAR,    y2d); }
int rv3129_set_weekday(rv3129_t* dev, uint8_t wd)    { return _set_field_and_write(dev, RV3129_TIME_DAY,     wd ); }

/* ==== 12h/24h & AM/PM ==== */
int rv3129_is_12h(rv3129_t* dev, bool* is12h)
{
    if (!dev || !is12h) return (int)ERROR_INVALID_PARAM;
    uint8_t h;
    int st = rv3129_read_reg(dev, RV3129_HOURS, &h);
    if (st != (int)ERROR_OK) return st;
    *is12h = ((h >> HOURS_12_24) & 0x1u) ? true : false;
    return (int)ERROR_OK;
}

int rv3129_is_pm(rv3129_t* dev, bool* is_pm)
{
    if (!dev || !is_pm) return (int)ERROR_INVALID_PARAM;
    uint8_t h;
    int st = rv3129_read_reg(dev, RV3129_HOURS, &h);
    if (st != (int)ERROR_OK) return st;
    bool is12=false; (void)rv3129_is_12h(dev,&is12);
    *is_pm = (is12 && ((h >> HOURS_AM_PM) & 0x1u)) ? true : false;
    return (int)ERROR_OK;
}

int rv3129_set_12h(rv3129_t* dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    uint8_t h;
    int st = rv3129_read_reg(dev, RV3129_HOURS, &h);
    if (st != (int)ERROR_OK) return st;

    if (h & (1u<<HOURS_12_24)) return (int)ERROR_OK; /* đã 12h */

    uint8_t hour24 = rv3129_bcd_to_dec(h & 0x3F);
    bool pm = false;
    if (hour24 == 0) { hour24 = 12; pm = false; }
    else if (hour24 == 12) { pm = true; }
    else if (hour24 > 12) { hour24 -= 12; pm = true; }

    uint8_t hour12_bcd = rv3129_dec_to_bcd(hour24);
    hour12_bcd |= (1u<<HOURS_12_24);
    if (pm) hour12_bcd |= (1u<<HOURS_AM_PM);
    return rv3129_write_reg(dev, RV3129_HOURS, hour12_bcd);
}

int rv3129_set_24h(rv3129_t* dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    uint8_t h;
    int st = rv3129_read_reg(dev, RV3129_HOURS, &h);
    if (st != (int)ERROR_OK) return st;

    if ((h & (1u<<HOURS_12_24)) == 0) return (int)ERROR_OK; /* đã 24h */

    bool pm = (h & (1u<<HOURS_AM_PM)) != 0;
    uint8_t core = h & 0x1F;
    uint8_t hour12 = rv3129_bcd_to_dec(core);
    uint8_t hour24 = hour12;

    if (pm) hour24 = (uint8_t)(hour12 + 12);
    if (hour24 == 24) hour24 = 12;
    if (hour12 == 12 && !pm) hour24 = 0;

    uint8_t b = rv3129_dec_to_bcd(hour24);
    b &= (uint8_t)~(1u<<HOURS_12_24);
    return rv3129_write_reg(dev, RV3129_HOURS, b);
}

/* ==== Temperature ==== */
int rv3129_get_temp(rv3129_t* dev, int8_t* temp_c)
{
    if (!dev || !temp_c) return (int)ERROR_INVALID_PARAM;
    uint8_t raw;
    int st = rv3129_read_reg(dev, RV3129_TEMP, &raw);
    if (st != (int)ERROR_OK) return st;
    if (raw < 60) *temp_c = (int8_t)(-(int8_t)(60 - raw));
    else          *temp_c = (int8_t)(raw - 60);
    return (int)ERROR_OK;
}

/* ==== Alarm ==== */
int rv3129_set_alarm_hms_dmYwd(rv3129_t* dev,
                               uint8_t sec, uint8_t min, uint8_t hour,
                               uint8_t date, uint8_t weekday, uint8_t month, uint8_t year2d)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    uint8_t a[RV3129_ALARM_ARRAY_LEN];
    a[RV3129_TIME_SECONDS] = rv3129_dec_to_bcd(sec);
    a[RV3129_TIME_MINUTES] = rv3129_dec_to_bcd(min);
    a[RV3129_TIME_HOURS]   = rv3129_dec_to_bcd(hour);
    a[RV3129_TIME_DATE]    = rv3129_dec_to_bcd(date);
    a[RV3129_TIME_DAY]     = rv3129_dec_to_bcd(weekday);
    a[RV3129_TIME_MONTH]   = rv3129_dec_to_bcd(month);
    a[RV3129_TIME_YEAR]    = rv3129_dec_to_bcd(year2d);
    return rv3129_write_regs(dev, RV3129_SECONDS_ALM, a, RV3129_ALARM_ARRAY_LEN);
}

int rv3129_alarm_enable_mask(rv3129_t* dev, uint8_t enableBits)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    uint8_t a[RV3129_ALARM_ARRAY_LEN];
    int st = rv3129_read_regs(dev, RV3129_SECONDS_ALM, a, RV3129_ALARM_ARRAY_LEN);
    if (st != (int)ERROR_OK) return st;

    for (uint8_t i=0;i<RV3129_ALARM_ARRAY_LEN;i++) {
        uint8_t en = (enableBits >> i) & 0x1u;
        a[i] = (uint8_t)(a[i] | (en<<7));
    }
    return rv3129_write_regs(dev, RV3129_SECONDS_ALM, a, RV3129_ALARM_ARRAY_LEN);
}

int rv3129_alarm_get_mode(rv3129_t* dev, uint8_t* modeBits)
{
    if (!dev || !modeBits) return (int)ERROR_INVALID_PARAM;
    uint8_t a[RV3129_ALARM_ARRAY_LEN];
    int st = rv3129_read_regs(dev, RV3129_SECONDS_ALM, a, RV3129_ALARM_ARRAY_LEN);
    if (st != (int)ERROR_OK) return st;

    uint8_t m=0;
    for (uint8_t i=0;i<RV3129_ALARM_ARRAY_LEN;i++)
        m |= (uint8_t)(((a[i]>>7)&1u) << i);
    *modeBits = m;
    return (int)ERROR_OK;
}

int rv3129_alarm_flag_get(rv3129_t* dev, bool* flag)
{
    if (!dev || !flag) return (int)ERROR_INVALID_PARAM;
    uint8_t v;
    int st = rv3129_read_reg(dev, RV3129_CTRL_INT_FLAG, &v);
    if (st != (int)ERROR_OK) return st;
    *flag = (v & 0x01u) ? true : false;
    return (int)ERROR_OK;
}

int rv3129_alarm_int_enable(rv3129_t* dev, bool en)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    uint8_t v;
    int st = rv3129_read_reg(dev, RV3129_CTRL_INT, &v);
    if (st != (int)ERROR_OK) return st;
    v = (uint8_t)((v & ~(1u<<0)) | ((en?1u:0u)<<0));
    return rv3129_write_reg(dev, RV3129_CTRL_INT, v);
}

/* ==== Timer ==== */
int rv3129_timer_set(rv3129_t* dev, uint16_t t)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    uint8_t b[2] = { (uint8_t)(t & 0xFFu), (uint8_t)(t>>8) };
    return rv3129_write_regs(dev, RV3129_TIME_LOW, b, 2);
}

int rv3129_timer_flag_get(rv3129_t* dev, bool* flag)
{
    if (!dev || !flag) return (int)ERROR_INVALID_PARAM;
    uint8_t v;
    int st = rv3129_read_reg(dev, RV3129_CTRL_INT_FLAG, &v);
    if (st != (int)ERROR_OK) return st;
    *flag = ((v>>1)&1u) ? true : false;
    return (int)ERROR_OK;
}

int rv3129_timer_int_enable(rv3129_t* dev, bool en)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    uint8_t v;
    int st = rv3129_read_reg(dev, RV3129_CTRL_INT, &v);
    if (st != (int)ERROR_OK) return st;
    v = (uint8_t)((v & ~(1u<<1)) | ((en?1u:0u)<<1));
    return rv3129_write_reg(dev, RV3129_CTRL_INT, v);
}

/* ==== Control reset ==== */
int rv3129_system_reset(rv3129_t* dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    return rv3129_write_reg(dev, RV3129_CTRL_RESET, 0x10);
}
