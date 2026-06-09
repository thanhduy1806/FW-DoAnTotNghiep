#ifndef ADG1414_H
#define ADG1414_H

#include <stdint.h>
#include <stdbool.h>
#include "spi_io.h"
#include "do.h"

/* Số chip tối đa trong chuỗi (bằng với bản cũ để an toàn) */
#define ADG1414_CHAIN_NUM_CHIPS_MAX    8
/* Tựa như bản cũ: hai cấu hình chuỗi “internal”/“external” */
#define INTERNAL_CHAIN_SWITCH_NUM      3
#define EXTERNAL_CHAIN_SWITCH_NUM      1
#define INTERNAL_CHAIN_CHANNEL_NUM     24
#define EXTERNAL_CHAIN_CHANNEL_NUM     8

typedef struct {
    spi_io_t   *spi;                                   /* bus SPI (do bạn quản lý) */
    do_t       *cs;                                    /* chân CS (Digital Output) */
    uint8_t     num_of_sw;                             /* số “chip/byte” trong chuỗi */
    uint8_t     switch_state[ADG1414_CHAIN_NUM_CHIPS_MAX]; /* cache trạng thái từng byte */
} adg1414_dev_t;

/* Khởi tạo chuỗi ADG1414:
 *  - spi  : bus SPI đã init (spi_io_init + cấu hình SPI ở nơi khác)
 *  - cs   : thông tin cổng/pin CS (port/pin) — phải là output
 *  - num  : số byte cần đẩy trong chuỗi (6 cho INTERNAL, 1 cho EXTERNAL, ... )
 * Ghi chú: Driver KHÔNG cấu hình mode SPI, chỉ sử dụng API spi_io_* để truyền.
 */
int adg1414_chain_init(adg1414_dev_t *dev, spi_io_t *spi,  do_t *cs, uint8_t num);

/* Bật một “kênh” theo mapping logic cũ:
 * - Với INTERNAL: chỉ cho phép một kênh hoạt động; channel_num = 1..36 (0 = off all)
 * - Với EXTERNAL: cho phép 0..8 (0 = off all), 1..8 bật bit tương ứng trong byte đầu
 */
int adg1414_chain_sw_on(adg1414_dev_t *dev, uint8_t channel_num);

/* Tắt một kênh (nếu hợp lệ). Với INTERNAL sẽ chỉ xóa bit tương ứng, không bật bit khác */
int adg1414_chain_sw_off(adg1414_dev_t *dev, uint8_t channel_num);

/* Tắt tất cả kênh */
int adg1414_chain_all_sw_off(adg1414_dev_t *dev);

int adg1414_manual_sw_on(adg1414_dev_t *dev, uint8_t channel_num);

int adg1414_manual_sw_off(adg1414_dev_t *dev, uint8_t channel_num);

#endif /* ADG1414_H */
