#ifndef AS3004_H
#define AS3004_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "spi_io.h"
#include "do.h"
#include "define.h"

/* Dung lượng AS3004204: 4 Mbit = 512 KiB = 2^19 bytes -> địa chỉ 24-bit */
#define AS3004_SIZE_BYTES        (512u * 1024u)
#define AS3004_ADDR_MAX          (AS3004_SIZE_BYTES - 1u)

/* Mã lệnh chuẩn (SPI 1-1-1) */
#define AS3004_CMD_READ_ID       0x9Fu  /* JEDEC ID (3 byte) */
#define AS3004_CMD_RDSR          0x05u  /* Read Status Register (1 byte) */
#define AS3004_CMD_WRSR          0x01u  /* Write Status Register (1 byte) */
#define AS3004_CMD_WREN          0x06u  /* Write Enable */
#define AS3004_CMD_WRDI          0x04u  /* Write Disable */
#define AS3004_CMD_READ          0x03u  /* Read (no dummy) */
#define AS3004_CMD_FAST_READ     0x0Bu  /* Fast Read (có dummy) */
#define AS3004_CMD_WRITE         0x02u  /* Write */

#define AS3004_CMD_RD_VOL        0x85u  /* Read Volatile Configuration */
#define AS3004_CMD_WR_VOL        0x81u  /* Write Volatile Configuration */
#define AS3004_CMD_RD_NVOL       0xB5u  /* Read Non-Volatile Configuration */
#define AS3004_CMD_WR_NVOL       0xB1u  /* Write Non-Volatile Configuration */

#define AS3004_CMD_RD_FLAGS      0x70u  /* Read Flags */
#define AS3004_CMD_CLR_FLAGS     0x50u  /* Clear Flags */

/* Trạng thái (Status Register) – bit phổ biến */
#define AS3004_SR_WIP            (1u << 0)  /* Write in progress (tùy dòng MRAM) */
#define AS3004_SR_WEL            (1u << 1)  /* Write enable latch */

/* Cấu hình driver */
typedef struct {
    spi_io_t *spi;           /* bus SPI do bạn quản lý */
    do_t      cs;            /* chân CS: active-low */
    uint8_t   dummy_cycles;  /* số byte dummy cho FAST READ (thường 1 byte = 8 cycles) */
} as3004_t;

/* ====== API ====== */
int as3004_init(as3004_t *dev, spi_io_t *spi, const do_t *cs);

/* Lệnh cơ bản */
int as3004_read_id(as3004_t *dev, uint8_t jedec_id[3]);
int as3004_read_status(as3004_t *dev, uint8_t *sr);
int as3004_write_status(as3004_t *dev, uint8_t sr);
int as3004_write_enable(as3004_t *dev);
int as3004_write_disable(as3004_t *dev);

/* Đọc/ghi dữ liệu */
int as3004_read     (as3004_t *dev, uint32_t addr, uint8_t *buf, size_t len);          /* 0x03 */
int as3004_read_fast(as3004_t *dev, uint32_t addr, uint8_t *buf, size_t len);          /* 0x0B + dummy */
int as3004_write    (as3004_t *dev, uint32_t addr, const uint8_t *buf, size_t len);    /* 0x02 */

/* Volatile/Non-Volatile Configuration */
int as3004_read_vol (as3004_t *dev, uint8_t index, uint8_t *buf, size_t len);
int as3004_write_vol(as3004_t *dev, uint8_t index, const uint8_t *buf, size_t len);
int as3004_read_nvol (as3004_t *dev, uint8_t index, uint8_t *buf, size_t len);
int as3004_write_nvol(as3004_t *dev, uint8_t index, const uint8_t *buf, size_t len);

/* Flags */
int as3004_read_flags(as3004_t *dev, uint8_t *flags);
int as3004_clear_flags(as3004_t *dev);

/* Tiện ích: đặt số byte dummy cho FAST READ (0 hoặc 1 …) */
static inline void as3004_set_dummy_bytes(as3004_t *dev, uint8_t n_dummy_bytes) {
    if (dev) dev->dummy_cycles = n_dummy_bytes;
}

#endif /* AS3004_H */
