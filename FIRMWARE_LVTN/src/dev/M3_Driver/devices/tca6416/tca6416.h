#ifndef TCA6416A_H
#define TCA6416A_H

#include <stdint.h>
#include "i2c_io.h"

/* TCA6416A register map */
#define TCA6416A_REG_INPUT0     0x00
#define TCA6416A_REG_INPUT1     0x01
#define TCA6416A_REG_OUTPUT0    0x02
#define TCA6416A_REG_OUTPUT1    0x03
#define TCA6416A_REG_POL0       0x04
#define TCA6416A_REG_POL1       0x05
#define TCA6416A_REG_CFG0       0x06
#define TCA6416A_REG_CFG1       0x07

/* Bit helper */
#ifndef BIT
#define BIT(n) (1u << (n))
#endif

/* Pin direction: 1 = input, 0 = output (đúng semantics của thanh ghi CONFIG) */
typedef enum {
    TCA6416A_MODE_OUTPUT = 0,
    TCA6416A_MODE_INPUT  = 1
} tca6416a_mode_t;

typedef struct {
    i2c_io_t*  bus;       /* Con trỏ đến bus I2C do người dùng quản lý */
    uint8_t    addr7;     /* Địa chỉ 7-bit (0x20..0x27 tùy cấu hình A[2:0]) */
    uint16_t   cache_out; /* Cache OUTPUT (để giữ mức các chân output) */
    uint16_t   cache_cfg; /* Cache CONFIG (1=input, 0=output) */
} tca6416a_t;

/* Khởi tạo và probe thiết bị: trả 0 nếu OK, <0 nếu lỗi */
int tca6416a_init(tca6416a_t* dev, i2c_io_t* bus, uint8_t addr7);

/* Đọc/ghi 16-bit cổng dữ liệu (P0..P15), và 16-bit cấu hình hướng */
int tca6416a_read_inputs (tca6416a_t* dev, uint16_t* in16);           /* đọc INPUT */
int tca6416a_write_outputs(tca6416a_t* dev, uint16_t out16);          /* ghi OUTPUT */
int tca6416a_read_outputs (tca6416a_t* dev, uint16_t* out16);         /* đọc lại cache OUTPUT */
int tca6416a_write_modes  (tca6416a_t* dev, uint16_t mode16);         /* 1=input */
int tca6416a_read_modes   (tca6416a_t* dev, uint16_t* mode16);

/* Per-pin tiện ích */
int tca6416a_pin_mode (tca6416a_t* dev, uint8_t pin, tca6416a_mode_t mode);
int tca6416a_pin_write(tca6416a_t* dev, uint8_t pin, uint8_t level);  /* 0/1 */
int tca6416a_pin_read (tca6416a_t* dev, uint8_t pin);                  /* trả 0/1, <0 nếu lỗi */

/* (tùy chọn) đảo cực đọc vào */
int tca6416a_write_polarity(tca6416a_t* dev, uint16_t pol16);
int tca6416a_read_polarity (tca6416a_t* dev, uint16_t* pol16);

#endif /* TCA6416A_H */
