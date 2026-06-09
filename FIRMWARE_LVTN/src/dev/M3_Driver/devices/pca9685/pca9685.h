#ifndef PCA9685_H
#define PCA9685_H

#include <stdint.h>
#include "i2c_io.h"

/* ---------------- Register map ---------------- */
#define PCA9685_MODE1            0x00
#define PCA9685_MODE2            0x01
#define PCA9685_LED0_ON_L        0x06
#define PCA9685_ALL_LED_ON_L     0xFA
#define PCA9685_ALL_LED_OFF_L    0xFC
#define PCA9685_PRESCALE         0xFE

/* ---------------- MODE1 bits ---------------- */
#define MODE1_RESTART  0x80
#define MODE1_EXTCLK   0x40
#define MODE1_AI       0x20    /* Auto-Increment */
#define MODE1_SLEEP    0x10
#define MODE1_ALLCALL  0x01

/* ---------------- MODE2 bits ---------------- */
#define MODE2_INVRT    0x10
#define MODE2_OUTDRV   0x04    /* 1: Totem-pole, 0: Open-drain */

/* Struct thiết bị tối giản */
typedef struct {
    i2c_io_t* bus;     /* Bus I2C do bạn quản lý và khởi tạo */
    uint8_t   addr7;   /* Địa chỉ 7-bit của PCA9685 (ví dụ 0x40) */
} pca9685_t;

/* (tùy chọn) delay hook – override ở platform nếu cần delay thực */
__attribute__((weak)) void pca9685_delay_ms(uint32_t ms);

/* ---------------- API chính ---------------- */
/* 1) Khởi động chip: bật AI, OUTDRV, clear outputs, soft-restart
 *    Trả về: ERROR_OK nếu thành công; lỗi I2C -> ERROR_I2C_TIMEOUT; tham số -> ERROR_INVALID_PARAM
 */
int pca9685_init_min(pca9685_t* dev, i2c_io_t* bus, uint8_t addr7);

/* 2) Đặt tần số PWM toàn chip (Hz), hợp lệ ~24..1526 (sẽ được clamp)
 *    Trả về: ERROR_OK / ERROR_I2C_TIMEOUT / ERROR_INVALID_PARAM
 */
int pca9685_set_freq_hz(pca9685_t* dev, uint16_t freq_hz);

/* 3) Đặt duty cho kênh (0..15) theo phần nghìn (0..1000).
 *    0    -> FULL OFF; 1000 -> FULL ON; khác -> PWM (ON=0, OFF=counts)
 */
int pca9685_set_duty_permille(pca9685_t* dev, uint8_t ch, uint16_t duty_permil);

/* Tiện: đặt theo phần trăm (0..100) – wrapper cho permille */
static inline int pca9685_set_duty_percent(pca9685_t* dev, uint8_t ch, uint8_t duty_percent) {
    if (duty_percent > 100) duty_percent = 100;
    return pca9685_set_duty_permille(dev, ch, (uint16_t)duty_percent * 10u);
}

/* ---------------- Bật/Tắt toàn bộ ngõ ra ---------------- */
int pca9685_output_disable(pca9685_t* dev);  /* FULL OFF toàn bộ */
int pca9685_output_enable (pca9685_t* dev);  /* Clear FULL OFF toàn bộ */

/* ---------------- Bật/Tắt từng kênh ---------------- */
int pca9685_channel_disable(pca9685_t* dev, uint8_t ch);   /* FULL OFF kênh */
int pca9685_channel_enable (pca9685_t* dev, uint8_t ch);   /* Clear FULL ON/OFF -> PWM bình thường */

/* (tuỳ chọn) ép FULL ON/OFF riêng kênh */
int pca9685_channel_full_on (pca9685_t* dev, uint8_t ch);  /* FORCE ON */
int pca9685_channel_full_off(pca9685_t* dev, uint8_t ch);  /* FORCE OFF (alias của disable) */

#endif /* PCA9685_MIN_H */
