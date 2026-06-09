#ifndef AD4114_H
#define AD4114_H

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "spi_io.h"
#include "do.h"


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ==== Register addresses (reg_addr) ==== */
#define AD4114_RA_STATUS        0x00
#define AD4114_RA_ADCMODE       0x01
#define AD4114_RA_IFMODE        0x02
#define AD4114_RA_DATA          0x04
#define AD4114_RA_GPIOCON       0x06
#define AD4114_RA_ID            0x07
#define AD4114_RA_CH(n)         (0x10u + (uint8_t)(n))   /* n=0..15 */
#define AD4114_RA_SETUPCON(n)   (0x20u + (uint8_t)(n))   /* n=0..7  */
#define AD4114_RA_FILTCON(n)    (0x28u + (uint8_t)(n))   /* n=0..7  */
#define AD4114_RA_OFFSET(n)     (0x30u + (uint8_t)(n))   /* 24-bit  */
#define AD4114_RA_GAIN(n)       (0x38u + (uint8_t)(n))   /* 24-bit  */

/* ==== IFMODE bits ==== */
#define AD4114_IF_CONTREAD      (1u << 7)
#define AD4114_IF_DATA_STAT     (1u << 6)
#define AD4114_IF_REG_CHECK     (1u << 5)
#define AD4114_IF_CRC_EN        (1u << 2)
#define AD4114_IF_WL16          (1u << 0)

/* ==== ADCMODE bits ==== */
#define AD4114_ADCMODE_REF_EN     (1u << 15)
#define AD4114_ADCMODE_SING_CYC   (1u << 13)
#define AD4114_ADCMODE_MODE_SHIFT 4
#define AD4114_ADCMODE_MODE_MASK  (7u << AD4114_ADCMODE_MODE_SHIFT)
#define AD4114_MODE_CONTINUOUS    (0u << AD4114_ADCMODE_MODE_SHIFT)
#define AD4114_MODE_SINGLE        (1u << AD4114_ADCMODE_MODE_SHIFT)
#define AD4114_MODE_STANDBY       (2u << AD4114_ADCMODE_MODE_SHIFT)
#define AD4114_MODE_POWER_DOWN    (3u << AD4114_ADCMODE_MODE_SHIFT)
#define AD4114_MODE_INTER_OFFSET  (4u << AD4114_ADCMODE_MODE_SHIFT)
#define AD4114_MODE_INTER_GAIN    (5u << AD4114_ADCMODE_MODE_SHIFT)
#define AD4114_MODE_SYSTEM_OFFSET (6u << AD4114_ADCMODE_MODE_SHIFT)
#define AD4114_MODE_SYSTEM_GAIN   (7u << AD4114_ADCMODE_MODE_SHIFT)

/* ==== STATUS bits ==== */
#define AD4114_STATUS_RDY        (1u << 7)   /* 0 = data ready */
#define AD4114_STATUS_CH_MASK    (0x0Fu)     /* when DATA_STAT=1, lower nibble = channel */

/* ==== SETUP bits ==== */
#define AD4114_SETUP_BI_UNIPOLAR(n)     ((n) << 12)
#define AD4114_SETUP_REFBUF_PN(n)      ((n) << 10)
#define AD4114_SETUP_INBUF(n)           ((n) << 8)
#define AD4114_SETUP_REF_SEL(n)         ((n) << 4)

/* ==== CHx fields ==== */
#define AD4114_CH_EN             (1u << 15)
#define AD4114_CH_SETUP_SHIFT    12
#define AD4114_CH_SETUP_MASK     (0x7u << AD4114_CH_SETUP_SHIFT)
#define AD4114_CH_INPUT_SHIFT    0
#define AD4114_CH_INPUT_MASK     0x03FFu

/* Helpers */
#define AD4114_INPUT_MAP(ainp, ainm)  ( ((uint16_t)((ainp)&0x1F) << 5) | ((uint16_t)((ainm)&0x1F) << 0) )

/* Counts */
#define AD4114_NUM_CHANNELS     16u
#define AD4114_NUM_SETUPS        8u

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct
{
    spi_io_t *spi;
    do_t     *cs;       /* CS pin (digital output), idle = HIGH */
    
    uint16_t  ifmode;   /* cache IFMODE */
    uint16_t  adcmode;  /* cache ADCMODE */
} ad4114_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ====== Core API ====== */
uint32_t ad4114_init(ad4114_t* p_dev, spi_io_t *spi, do_t *cs);
uint32_t ad4114_sw_reset(ad4114_t* p_dev);
uint32_t ad4114_read_id(ad4114_t* p_dev, uint16_t *id_out);

/* Mode shortcuts */
uint32_t ad4114_set_ifmode(ad4114_t* p_dev, uint16_t ifmode);
uint32_t ad4114_set_adcmode(ad4114_t* p_dev, uint16_t adcmode);

/* Channel config */
uint32_t ad4114_config_channel(ad4114_t* p_dev, uint8_t channel, bool enable,
                               uint16_t input_map, uint8_t setup);

/* Data read helpers */
uint32_t ad4114_read_channel_once(ad4114_t* p_dev, uint8_t channel, uint32_t timeout_us, uint32_t *raw24);
uint32_t ad4114_read_data_wait(ad4114_t* p_dev, uint32_t timeout_us, uint32_t *raw24, uint8_t *status_opt);
uint32_t ad4114_data_to_vin(uint32_t raw24, uint32_t gain24, uint32_t offset24,
                            bool bipolar, float vref, float* vin_out);

/* ====== Helper APIs ====== */
uint32_t ad4114_channel_select_inputs(ad4114_t* p_dev, uint8_t channel, uint8_t ainp, uint8_t ainm);
uint32_t ad4114_channel_select_setup (ad4114_t* p_dev, uint8_t channel, uint8_t setup);
uint32_t ad4114_channels_enable_mask (ad4114_t* p_dev, uint16_t mask);
uint32_t ad4114_channels_disable_mask(ad4114_t* p_dev, uint16_t mask);
uint32_t ad4114_setup_write (ad4114_t* p_dev, uint8_t setup, uint16_t setupcon);
uint32_t ad4114_setup_read  (ad4114_t* p_dev, uint8_t setup, uint16_t *setupcon);
uint32_t ad4114_filt_write  (ad4114_t* p_dev, uint8_t setup, uint16_t filtcon);
uint32_t ad4114_filt_read   (ad4114_t* p_dev, uint8_t setup, uint16_t *filtcon);
uint32_t ad4114_offset_write(ad4114_t* p_dev, uint8_t setup, uint32_t offset24);
uint32_t ad4114_offset_read (ad4114_t* p_dev, uint8_t setup, uint32_t *offset24);
uint32_t ad4114_gain_write  (ad4114_t* p_dev, uint8_t setup, uint32_t gain24);
uint32_t ad4114_gain_read   (ad4114_t* p_dev, uint8_t setup, uint32_t *gain24);
uint32_t ad4114_set_mode_continuous(ad4114_t* p_dev, bool ref_en);
uint32_t ad4114_set_mode_single    (ad4114_t* p_dev, bool single_cycle, bool ref_en);

/* Đọc 1 mẫu cho tất cả kênh đang enable */
uint32_t ad4114_read_all(ad4114_t* p_dev,
                         uint32_t timeout_us_per_sample,
                         uint16_t *out_ch_mask,
                         uint32_t samples[AD4114_NUM_CHANNELS]);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* AD4114_H */