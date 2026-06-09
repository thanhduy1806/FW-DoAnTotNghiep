#ifndef RV3129_H
#define RV3129_H

#include <stdint.h>
#include <stdbool.h>
#include "i2c_io.h"

/* Địa chỉ 7-bit của RV3129 */
#ifndef RV3129_ADDR
#define RV3129_ADDR  (0x56u)
#endif

/*********** Register map (theo mã gốc) ***********/
/* Control Page */
#define RV3129_CTRL_1           0x00
#define RV3129_CTRL_INT         0x01
#define RV3129_CTRL_INT_FLAG    0x02
#define RV3129_CTRL_STATUS      0x03
#define RV3129_CTRL_RESET       0x04

/* Clock Page */
#define RV3129_SECONDS          0x08
#define RV3129_MINUTES          0x09
#define RV3129_HOURS            0x0A
#define RV3129_DATE             0x0B
#define RV3129_WEEKDAYS         0x0C
#define RV3129_MONTHS           0x0D
#define RV3129_YEARS            0x0E

/* Hours bits */
#define HOURS_12_24             6   /* 1=12h, 0=24h */
#define HOURS_AM_PM             5   /* only in 12h mode: 0=AM,1=PM */

/* Alarm Page */
#define RV3129_SECONDS_ALM      0x10
#define RV3129_MINUTES_ALM      0x11
#define RV3129_HOURS_ALM        0x12
#define RV3129_DATE_ALM         0x13
#define RV3129_WEEKDAYS_ALM     0x14
#define RV3129_MONTHS_ALM       0x15
#define RV3129_YEARS_ALM        0x16

/* Timer Page */
#define RV3129_TIME_LOW         0x18
#define RV3129_TIME_HIGH        0x19

/* Temperature Page */
#define RV3129_TEMP             0x20

/* RAM Page */
#define RV3129_USER_RAM         0x38

/* Độ dài mảng thời gian/alarm */
#define RV3129_TIME_ARRAY_LEN   7u
#define RV3129_ALARM_ARRAY_LEN  7u

/* Thứ tự mảng thời gian */
typedef enum {
    RV3129_TIME_SECONDS = 0,
    RV3129_TIME_MINUTES = 1,
    RV3129_TIME_HOURS   = 2,
    RV3129_TIME_DATE    = 3,
    RV3129_TIME_DAY     = 4,  /* weekday */
    RV3129_TIME_MONTH   = 5,
    RV3129_TIME_YEAR    = 6   /* 00..99 */
} rv3129_time_idx_t;

/* Thiết bị */
typedef struct {
    i2c_io_t* bus;
    uint8_t   addr7;                      /* 7-bit address (0x56) */
    uint8_t   time[RV3129_TIME_ARRAY_LEN];/* cache LAST read */
} rv3129_t;

/* ====== API ====== */
/* Khởi tạo: chỉ gán bus/addr; không tự động thay đổi mode 12/24h */
int  rv3129_init(rv3129_t* dev, i2c_io_t* bus, uint8_t addr7);

/* Đọc nhiều/ghi nhiều thanh ghi */
int  rv3129_read_regs (rv3129_t* dev, uint8_t reg, uint8_t* dst, uint8_t len);
int  rv3129_write_regs(rv3129_t* dev, uint8_t reg, const uint8_t* src, uint8_t len);

/* Đọc/ghi 1 byte */
int  rv3129_read_reg (rv3129_t* dev, uint8_t reg, uint8_t* val);
int  rv3129_write_reg(rv3129_t* dev, uint8_t reg, uint8_t  val);

/* Chuyển đổi BCD/DEC */
uint8_t rv3129_bcd_to_dec(uint8_t v);
uint8_t rv3129_dec_to_bcd(uint8_t v);

/* Cập nhật cache thời gian từ chip -> dev->time */
int  rv3129_update_time(rv3129_t* dev);

/* Lấy trường đơn từ cache (sau update_time) – giá trị DEC */
uint8_t rv3129_get_seconds(const rv3129_t* dev);
uint8_t rv3129_get_minutes(const rv3129_t* dev);
uint8_t rv3129_get_hours  (const rv3129_t* dev);   /* nếu 12h, trả 1..12 (bit AM/PM đã clear) */
uint8_t rv3129_get_weekday(const rv3129_t* dev);   /* 1..7 tùy cấu hình */
uint8_t rv3129_get_date   (const rv3129_t* dev);   /* 1..31 */
uint8_t rv3129_get_month  (const rv3129_t* dev);   /* 1..12 */
uint8_t rv3129_get_year   (const rv3129_t* dev);   /* 0..99 */

/* Ghi thời gian (24h/12h theo trạng thái hiện tại của thanh ghi HOURS) */
int  rv3129_set_time(rv3129_t* dev,
                     uint8_t sec, uint8_t min, uint8_t hour,
                     uint8_t date, uint8_t month, uint16_t year, uint8_t weekday);

/* Đặt từng trường (ghi cả block để tránh race) */
int rv3129_set_seconds(rv3129_t* dev, uint8_t sec);
int rv3129_set_minutes(rv3129_t* dev, uint8_t min);
int rv3129_set_hours  (rv3129_t* dev, uint8_t hour);
int rv3129_set_date   (rv3129_t* dev, uint8_t date);
int rv3129_set_month  (rv3129_t* dev, uint8_t month);
int rv3129_set_year   (rv3129_t* dev, uint8_t year2d);
int rv3129_set_weekday(rv3129_t* dev, uint8_t weekday);

/* 12h/24h */
int  rv3129_is_12h (rv3129_t* dev, bool* is12h);
int  rv3129_set_12h(rv3129_t* dev);  /* chuyển sang 12h, bảo toàn giá trị giờ hiện tại */
int  rv3129_set_24h(rv3129_t* dev);  /* chuyển sang 24h, bảo toàn giá trị giờ hiện tại */
int  rv3129_is_pm  (rv3129_t* dev, bool* is_pm); /* chỉ hợp lệ ở 12h */

/* Nhiệt độ (theo map của mã gốc: 0..250 ↔ -60..+190°C) */
int  rv3129_get_temp(rv3129_t* dev, int8_t* temp_c);

/* Alarm cơ bản */
int  rv3129_set_alarm_hms_dmYwd(rv3129_t* dev,
                                uint8_t sec, uint8_t min, uint8_t hour,
                                uint8_t date, uint8_t weekday, uint8_t month, uint8_t year2d);
int  rv3129_alarm_enable_mask (rv3129_t* dev, uint8_t enableBits);
int  rv3129_alarm_get_mode    (rv3129_t* dev, uint8_t* modeBits);
int  rv3129_alarm_flag_get    (rv3129_t* dev, bool* flag);
int  rv3129_alarm_int_enable  (rv3129_t* dev, bool en);

/* Timer cơ bản (16-bit), và INT */
int  rv3129_timer_set        (rv3129_t* dev, uint16_t t);
int  rv3129_timer_flag_get   (rv3129_t* dev, bool* flag);
int  rv3129_timer_int_enable (rv3129_t* dev, bool en);

/* Reset mềm qua thanh ghi control */
int  rv3129_system_reset(rv3129_t* dev);

#endif /* RV3129_H */
