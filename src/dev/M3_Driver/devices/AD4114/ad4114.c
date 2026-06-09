/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ad4114.h"
#include "define.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define AD4114_COMMS_RW_READ   0x40u  /* Bit6 = 1 (read), 0 = write */
#define AD4114_COMMS_WEN0      0x00u  /* Bit7 = WEN (keep 0) */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static inline void cs_active(ad4114_t* p_dev);
static inline void cs_idle  (ad4114_t* p_dev);
static uint32_t wr_frame(ad4114_t* p_dev, uint8_t reg_addr, const uint8_t *p_TX, uint32_t length);
static uint32_t rd_frame(ad4114_t* p_dev, uint8_t reg_addr, uint8_t* p_RX, uint32_t length);

/* Raw R/W */
// static uint32_t ad4114_write8 (ad4114_t* p_dev, uint8_t reg_addr, uint8_t TX_data);
static uint32_t ad4114_write16(ad4114_t* p_dev, uint8_t reg_addr, uint16_t TX_data);
static uint32_t ad4114_write24(ad4114_t* p_dev, uint8_t reg_addr, uint32_t TX_data);
static uint32_t ad4114_read8 (ad4114_t* p_dev, uint8_t reg_addr, uint8_t* p_RX);
static uint32_t ad4114_read16(ad4114_t* p_dev, uint8_t reg_addr, uint16_t* p_RX);
static uint32_t ad4114_read24(ad4114_t* p_dev, uint8_t reg_addr, uint32_t* p_RX);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ===== Core API ===== */

uint32_t ad4114_init(ad4114_t *p_dev, spi_io_t *spi, do_t *cs)
{
    if (!p_dev || !spi || !cs)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }
    
    // p_dev->spi      = spi;
    // p_dev->cs       = cs;
    // p_dev->ifmode   = 0;
    // p_dev->adcmode  = 0;

    cs_idle(p_dev);

    uint32_t rc = ad4114_sw_reset(p_dev);

    if (rc != (uint32_t)ERROR_OK)
    {
        return rc;
    }
    os_delay_ms(10);
    rc = ad4114_set_ifmode(p_dev, (uint16_t)(p_dev->ifmode | AD4114_IF_DATA_STAT));
    if (rc != (uint32_t)ERROR_OK)
    {
        return rc;
    }

    rc = ad4114_set_adcmode(p_dev, (uint16_t)((p_dev->adcmode & ~AD4114_ADCMODE_MODE_MASK) | AD4114_MODE_CONTINUOUS));
    return rc;
}

uint32_t ad4114_sw_reset(ad4114_t *p_dev)
{
    if (!p_dev || !p_dev->spi || !p_dev->cs)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint8_t TX_frame[8];
    
    for (uint8_t i=0; i<8; i++) 
    {
        TX_frame[i] = 0xFF;
    }

    cs_active(p_dev);
    uint32_t st = spi_io_write_sync(p_dev->spi, TX_frame, 8);
    cs_idle(p_dev);

    return (st == ERROR_OK) ? (uint32_t)ERROR_OK : st;
}

uint32_t ad4114_read_id(ad4114_t *p_dev, uint16_t *id_out)
{
    if (!p_dev || !id_out)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint32_t rc = ad4114_read16(p_dev, AD4114_RA_ID, id_out);

    if (rc != (uint32_t)ERROR_OK)
    {
        return rc;
    }

    return (uint32_t)ERROR_OK;
}

/* ===== Mode shortcuts ===== */

uint32_t ad4114_set_ifmode(ad4114_t *p_dev, uint16_t ifmode)
{
    if (!p_dev)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }
    
    uint32_t rc = ad4114_write16(p_dev, AD4114_RA_IFMODE, ifmode);

    if (rc == (uint32_t)ERROR_OK)
    {
        p_dev->ifmode = ifmode;
    }
    return rc;
}

uint32_t ad4114_set_adcmode(ad4114_t *p_dev, uint16_t adcmode)
{
    if (!p_dev)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }
    
    uint32_t rc = ad4114_write16(p_dev, AD4114_RA_ADCMODE, adcmode);

    if (rc == (uint32_t)ERROR_OK)
    {
        p_dev->adcmode = adcmode;
    }
    
    return rc;
}

/* ===== Channel config ===== */

uint32_t ad4114_config_channel(ad4114_t *p_dev, uint8_t channel, bool enable,
                               uint16_t input_map, uint8_t setup)
{
    if (!p_dev || channel >= AD4114_NUM_CHANNELS || setup >= AD4114_NUM_SETUPS)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint16_t v = 0;

    if (enable)
    {
        v |= AD4114_CH_EN;
    }

    v |= ((uint16_t)setup & 0x7u) << AD4114_CH_SETUP_SHIFT;
    v |= (input_map & AD4114_CH_INPUT_MASK) << AD4114_CH_INPUT_SHIFT;
    return ad4114_write16(p_dev, AD4114_RA_CH(channel), v);
}

/* ===== Wait for RDY then read DATA ===== */

// static uint32_t wait_rdy_low(ad4114_t *p_dev, uint32_t timeout_us)
// {
//     uint8_t st = 0xFF;
//     uint32_t wait_count = (timeout_us == 0) ? 20000u : timeout_us;

//     while (wait_count--)
//     {
//         uint32_t rc = ad4114_read8(p_dev, AD4114_RA_STATUS, &st);

//         if (rc != (uint32_t)ERROR_OK)
//         {
//             return rc;
//         }
        
//         if ((st & AD4114_STATUS_RDY) == 0)
//         {
//             return (uint32_t)ERROR_OK;
//         }
//     }

//     return (uint32_t)ERROR_TIMEOUT;
// }

uint32_t ad4114_read_data_wait(ad4114_t *p_dev, uint32_t timeout_us, uint32_t *p_raw24, uint8_t *p_status)
{
    if (!p_dev || !p_raw24 || !p_status)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    //uint32_t rc = wait_rdy_low(p_dev, timeout_us); 
    uint32_t rc = (uint32_t)ERROR_OK;
    if (rc != (uint32_t)ERROR_OK)
    {
        return rc;
    }

    if (p_dev->ifmode & AD4114_IF_DATA_STAT)
    {
        uint8_t b[4];

        uint32_t st = rd_frame(p_dev, AD4114_RA_DATA, b, 4);

        if (st != ERROR_OK)
        {
            return st;
        }

        *p_raw24 = ((uint32_t)b[0] << 16) | ((uint32_t)b[1] << 8) | b[2];

        *p_status = b[3];

        return (uint32_t)ERROR_OK;

        // return ad4114_read24(p_dev, AD4114_RA_DATA, p_raw24);
    }
    else
    {
        return ad4114_read24(p_dev, AD4114_RA_DATA, p_raw24);
    }
}

/* Tính VIN từ code 24-bit, GAINx, OFFSETx theo datasheet.
 * - raw24  : giá trị 24-bit từ thanh ghi DATA (0..0xFFFFFF)
 * - gain24 : giá trị 24-bit từ GAINx (0x000000..0xFFFFFF), chuẩn hóa theo 0x400000
 * - offset24: giá trị 24-bit từ OFFSETx (thường mặc định 0x800000)
 * - bipolar: true = bipolar (offset-binary), false = unipolar (straight-binary)
 * - vref   : điện áp tham chiếu (Volt)
 * - vin_out: kết quả (Volt)
 */
uint32_t ad4114_data_to_vin(uint32_t raw24,
                            uint32_t gain24,
                            uint32_t offset24,
                            bool     bipolar,
                            float    vref,
                            float   *vin_out)
{
    if (!vin_out || vref <= 0.0f) return (uint32_t)ERROR_INVALID_PARAM;

    /* chặn 24-bit, tránh tràn */
    raw24    &= 0xFFFFFFu;
    gain24   &= 0xFFFFFFu;
    offset24 &= 0xFFFFFFu;

    /* Hằng số theo datasheet */
    const float K_ATT   = 0.075f;           /* hệ số suy giảm nội trước hiệu chuẩn */
    const float TWO23   = 8388608.0f;       /* 2^23 */
    const float G_NORM  = 4194304.0f;       /* 0x400000 = 2^22 */

    /* Chuẩn hoá gain */
    float G = (gain24 > 0u) ? ((float)gain24 / G_NORM) : 1.0f;

    float term_off = ((float)((int32_t)(offset24) - (int32_t)0x800000)) / TWO23;
    float vin;

    if (!bipolar) {
        /* Unipolar: VIN = VREF/0.075 * [ Data/(G*2*2^23) + (OFFSET-0x800000)/2^23 ] */
        float term_data = ((float)raw24) / (G * 2.0f * TWO23);
        vin = (vref / K_ATT) * (term_data + term_off);
    } else {
        /* Bipolar: VIN = VREF/0.075 * [ (Data-0x800000)/(G*2^23) + (OFFSET-0x800000)/2^23 ] */
        float term_data = ((float)((int32_t)raw24 - (int32_t)0x800000)) / (G * TWO23);
        vin = (vref / K_ATT) * (term_data + term_off);
    }

    *vin_out = vin;
    return (uint32_t)ERROR_OK;
}

uint32_t ad4114_read_channel_once(ad4114_t *p_dev, uint8_t channel, uint32_t timeout_us, uint32_t *raw24)
{
    if (!p_dev || !raw24 || channel >= AD4114_NUM_CHANNELS) return (uint32_t)ERROR_INVALID_PARAM;

    for (uint8_t i = 0; i < AD4114_NUM_CHANNELS; ++i) {
        (void)ad4114_write16(p_dev, AD4114_RA_CH(i), 0x0000);
    }

    uint16_t v = 0;
    uint32_t rc = ad4114_read16(p_dev, AD4114_RA_CH(channel), &v);
    if (rc != (uint32_t)ERROR_OK) return rc;
    v |= AD4114_CH_EN;
    rc = ad4114_write16(p_dev, AD4114_RA_CH(channel), v);
    if (rc != (uint32_t)ERROR_OK) return rc;

    uint16_t m = p_dev->adcmode;
    m &= (uint16_t)~AD4114_ADCMODE_MODE_MASK;
    m |= AD4114_MODE_SINGLE;
    rc = ad4114_set_adcmode(p_dev, m);
    if (rc != (uint32_t)ERROR_OK) return rc;

    rc = ad4114_read_data_wait(p_dev, timeout_us ? timeout_us : 50000u, raw24, NULL);
    return rc;
}

/* ===== Helper APIs ===== */

uint32_t ad4114_channel_select_inputs(ad4114_t *p_dev, uint8_t channel, uint8_t ainp, uint8_t ainm)
{
    if (!p_dev || channel >= AD4114_NUM_CHANNELS)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint16_t v;
    uint32_t rc = ad4114_read16(p_dev, AD4114_RA_CH(channel), &v);

    if (rc != (uint32_t)ERROR_OK)
    {
        return rc;
    }

    v &= (uint16_t)~(AD4114_CH_INPUT_MASK << AD4114_CH_INPUT_SHIFT);
    v |= (uint16_t)(AD4114_INPUT_MAP(ainp, ainm) & AD4114_CH_INPUT_MASK);

    return ad4114_write16(p_dev, AD4114_RA_CH(channel), v);
}

uint32_t ad4114_channel_select_setup(ad4114_t *p_dev, uint8_t channel, uint8_t setup)
{
    if (!p_dev || channel >= AD4114_NUM_CHANNELS || setup >= AD4114_NUM_SETUPS)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint16_t v;
    uint32_t rc = ad4114_read16(p_dev, AD4114_RA_CH(channel), &v);

    if (rc != (uint32_t)ERROR_OK)
    {
        return rc;
    }

    v = (uint16_t)((v & (uint16_t)~AD4114_CH_SETUP_MASK) | (((uint16_t)setup << AD4114_CH_SETUP_SHIFT) & AD4114_CH_SETUP_MASK));
    return ad4114_write16(p_dev, AD4114_RA_CH(channel), v);
}

uint32_t ad4114_channels_enable_mask(ad4114_t *p_dev, uint16_t mask)
{
    if (!p_dev)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }
    
    for (uint8_t channel = 0; channel < AD4114_NUM_CHANNELS; ++channel)
    {
        uint16_t v;
        uint32_t rc = ad4114_read16(p_dev, AD4114_RA_CH(channel), &v);

        if (rc != (uint32_t)ERROR_OK)
        {
            return rc;
        }

        if (mask & (1u << channel))
        {
            v |= AD4114_CH_EN;
        }

        rc = ad4114_write16(p_dev, AD4114_RA_CH(channel), v);

        if (rc != (uint32_t)ERROR_OK)
        {
            return rc;
        }
    }
    return (uint32_t)ERROR_OK;
}

uint32_t ad4114_channels_disable_mask(ad4114_t *p_dev, uint16_t mask)
{
    if (!p_dev)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    } 
    for (uint8_t channel = 0; channel < AD4114_NUM_CHANNELS; ++channel)
    {
        uint16_t v;
        uint32_t rc = ad4114_read16(p_dev, AD4114_RA_CH(channel), &v);

        if (rc != (uint32_t)ERROR_OK)
        {
            return rc;
        }

        if (mask & (1u << channel))
        {
            v &= (uint16_t)~AD4114_CH_EN;
        }

        rc = ad4114_write16(p_dev, AD4114_RA_CH(channel), v);

        if (rc != (uint32_t)ERROR_OK)
        {
            return rc;
        }
    }
    return (uint32_t)ERROR_OK;
}

uint32_t ad4114_setup_write(ad4114_t *p_dev, uint8_t setup, uint16_t setupcon)
{
    if (!p_dev || setup >= AD4114_NUM_SETUPS)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }
    
    return ad4114_write16(p_dev, AD4114_RA_SETUPCON(setup), setupcon);
}

uint32_t ad4114_setup_read(ad4114_t *p_dev, uint8_t setup, uint16_t *setupcon)
{
    if (!p_dev || !setupcon || setup >= AD4114_NUM_SETUPS)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    return ad4114_read16(p_dev, AD4114_RA_SETUPCON(setup), setupcon);
}

uint32_t ad4114_filt_write(ad4114_t *p_dev, uint8_t setup, uint16_t filtcon)
{
    if (!p_dev || setup >= AD4114_NUM_SETUPS) return (uint32_t)ERROR_INVALID_PARAM;
    return ad4114_write16(p_dev, AD4114_RA_FILTCON(setup), filtcon);
}

uint32_t ad4114_filt_read(ad4114_t *p_dev, uint8_t setup, uint16_t *filtcon)
{
    if (!p_dev || !filtcon || setup >= AD4114_NUM_SETUPS) return (uint32_t)ERROR_INVALID_PARAM;
    return ad4114_read16(p_dev, AD4114_RA_FILTCON(setup), filtcon);
}

uint32_t ad4114_offset_write(ad4114_t *p_dev, uint8_t setup, uint32_t offset24)
{
    if (!p_dev || setup >= AD4114_NUM_SETUPS) return (uint32_t)ERROR_INVALID_PARAM;
    return ad4114_write24(p_dev, AD4114_RA_OFFSET(setup), offset24 & 0xFFFFFFu);
}

uint32_t ad4114_offset_read(ad4114_t *p_dev, uint8_t setup, uint32_t *offset24)
{
    if (!p_dev || !offset24 || setup >= AD4114_NUM_SETUPS) return (uint32_t)ERROR_INVALID_PARAM;
    return ad4114_read24(p_dev, AD4114_RA_OFFSET(setup), offset24);
}

uint32_t ad4114_gain_write(ad4114_t *p_dev, uint8_t setup, uint32_t gain24)
{
    if (!p_dev || setup >= AD4114_NUM_SETUPS)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    return ad4114_write24(p_dev, AD4114_RA_GAIN(setup), gain24 & 0xFFFFFFu);
}

uint32_t ad4114_gain_read(ad4114_t *p_dev, uint8_t setup, uint32_t *gain24)
{
    if (!p_dev || !gain24 || setup >= AD4114_NUM_SETUPS) return (uint32_t)ERROR_INVALID_PARAM;
    return ad4114_read24(p_dev, AD4114_RA_GAIN(setup), gain24);
}

/* 5) Shortcuts chế độ chuyển đổi */
uint32_t ad4114_set_mode_continuous(ad4114_t *p_dev, bool ref_en)
{
    if (!p_dev)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint16_t m = p_dev->adcmode;

    m &= (uint16_t)~AD4114_ADCMODE_MODE_MASK;
    m |= AD4114_MODE_CONTINUOUS;

    if (ref_en)
    {
        m |= AD4114_ADCMODE_REF_EN;
    }
    else
    {
        m &= (uint16_t)~AD4114_ADCMODE_REF_EN;
    }

    return ad4114_set_adcmode(p_dev, m);
}

uint32_t ad4114_set_mode_single(ad4114_t *p_dev, bool single_cycle, bool ref_en)
{
    if (!p_dev)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint16_t m = p_dev->adcmode;

    m &= (uint16_t)~AD4114_ADCMODE_MODE_MASK;
    m |= AD4114_MODE_SINGLE;

    if (single_cycle)
    {
        m |= AD4114_ADCMODE_SING_CYC;
    }
    else
    {
        m &= (uint16_t)~AD4114_ADCMODE_SING_CYC;
    }
    
    if (ref_en)
    {
        m |= AD4114_ADCMODE_REF_EN;
    }
    else
    {
        m &= (uint16_t)~AD4114_ADCMODE_REF_EN;
    }
        return ad4114_set_adcmode(p_dev, m);
}

/* 6) Đọc 1 mẫu cho tất cả kênh đang enable */
uint32_t ad4114_read_all(ad4114_t* p_dev,
                         uint32_t timeout_us_per_sample,
                         uint16_t* p_out_ch_mask,
                         uint32_t samples[AD4114_NUM_CHANNELS])
{
    if (!p_dev || !samples || !p_out_ch_mask)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    /* 1) Xác định các kênh đang enable */
    uint16_t enabled_mask = 0;
    for (uint8_t channel = 0; channel < AD4114_NUM_CHANNELS; ++channel)
    {
        uint16_t RX_data = 0;
        uint32_t rc = ad4114_read16(p_dev, AD4114_RA_CH(channel), &RX_data);

        if (rc != (uint32_t)ERROR_OK)
        {
            return rc;
        } 

        if (RX_data & AD4114_CH_EN)
        {
            enabled_mask |= (uint16_t)(1u << channel);
        }
    }

    if (enabled_mask == 0)
    {
        return (uint32_t)ERROR_NOT_READY;
    }

    *p_out_ch_mask = 0;

    /* 2) Bật tạm DATA_STAT nếu chưa bật */
    const bool had_data_stat = (p_dev->ifmode & AD4114_IF_DATA_STAT) != 0;
    if (!had_data_stat)
    {
        uint32_t rc = ad4114_set_ifmode(p_dev, (uint16_t)(p_dev->ifmode | AD4114_IF_DATA_STAT));

        if (rc != (uint32_t)ERROR_OK)
        {
            return rc;
        }
    }

    /* 3) Chuyển continuous mode (nếu chưa) */
    uint16_t adcmode_before = p_dev->adcmode;
    uint16_t adcmode_temp   = (uint16_t)((adcmode_before & (uint16_t)~AD4114_ADCMODE_MODE_MASK) | AD4114_MODE_CONTINUOUS);
    uint32_t rc = ad4114_set_adcmode(p_dev, adcmode_temp);
    if (rc != (uint32_t)ERROR_OK)
    {
        if (!had_data_stat)
        {
            ad4114_set_ifmode(p_dev, (uint16_t)(p_dev->ifmode & (uint16_t)~AD4114_IF_DATA_STAT));
        }

        if (p_dev->adcmode != adcmode_before)
        {
            ad4114_set_adcmode(p_dev, adcmode_before);
        }

        return rc;
    }

    /* 4) Đọc vòng: cần N mẫu với N = số kênh enable */
    uint8_t  seen[AD4114_NUM_CHANNELS] = {0};
    uint16_t remaining = 0;

    for (uint8_t channel = 0; channel < AD4114_NUM_CHANNELS; ++channel)
    {
        if (enabled_mask & (uint16_t)(1u << channel))
        {
            remaining++;
            samples[channel] = 0;
        } 
    }

    while (remaining)
    {
        uint8_t status = 0;
        uint32_t raw   = 0;

        rc = ad4114_read_data_wait(p_dev, timeout_us_per_sample, &raw, &status);

        if (rc != (uint32_t)ERROR_OK)
        {
            rc = (uint32_t)ERROR_TIMEOUT;

            if (!had_data_stat)
            {
                ad4114_set_ifmode(p_dev, (uint16_t)(p_dev->ifmode & (uint16_t)~AD4114_IF_DATA_STAT));
            }

            if (p_dev->adcmode != adcmode_before)
            {
                ad4114_set_adcmode(p_dev, adcmode_before);
            }

            return rc;
        }

        uint8_t channel = (uint8_t)(status & AD4114_STATUS_CH_MASK);

        if (channel >= AD4114_NUM_CHANNELS)
        {
            continue;
        }

        if ((enabled_mask & (uint16_t)(1u << channel)) && !seen[channel])
        {
            samples[channel] = raw;
            seen[channel]    = 1;
            remaining--;

            *p_out_ch_mask |= (uint16_t)(1u << channel);
        }
    }

    if (!had_data_stat)
    {
        ad4114_set_ifmode(p_dev, (uint16_t)(p_dev->ifmode & (uint16_t)~AD4114_IF_DATA_STAT));
    }

    if (p_dev->adcmode != adcmode_before)
    {
        ad4114_set_adcmode(p_dev, adcmode_before);
    }

    return (uint32_t)ERROR_OK;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static inline void cs_active(ad4114_t* p_dev)
{
    do_reset(p_dev->cs);
}

static inline void cs_idle  (ad4114_t* p_dev)
{
    do_set(p_dev->cs);
}

static uint32_t wr_frame(ad4114_t* p_dev, uint8_t reg_addr, const uint8_t *p_TX, uint32_t length_byte)
{
    uint8_t TX_frame[1 + 4];

    if (!p_dev || !p_dev->spi || !p_dev->cs || length_byte > 4)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    TX_frame[0] = (uint8_t)(AD4114_COMMS_WEN0 | (reg_addr & 0x3Fu)); /* write */

    for (size_t i = 0; i < length_byte; ++i)
    {
        TX_frame[1 + i] = p_TX[i];
    }

    cs_active(p_dev);

    uint32_t st = spi_io_write_sync(p_dev->spi, TX_frame, (uint32_t)(1 + length_byte));

    cs_idle(p_dev);

    return (st == ERROR_OK) ? (uint32_t)ERROR_OK : st;
}

static uint32_t rd_frame(ad4114_t* p_dev, uint8_t reg_addr, uint8_t* p_RX, uint32_t length_byte)
{
    if (!p_dev || !p_dev->spi || !p_dev->cs || !p_RX || length_byte == 0 || length_byte > 4)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    } 

    uint8_t CMD = (uint8_t)(AD4114_COMMS_WEN0 | AD4114_COMMS_RW_READ | (reg_addr & 0x3Fu));

    cs_active(p_dev);

    uint32_t st = spi_io_write_sync(p_dev->spi, &CMD, 1);
    if (st != ERROR_OK)
    {
        cs_idle(p_dev);
        return st;
    }

    st = spi_io_read_sync(p_dev->spi, p_RX, (uint32_t)length_byte);

    cs_idle(p_dev);

    return (st == ERROR_OK) ? (uint32_t)ERROR_OK : st;
}

// static uint32_t ad4114_write8 (ad4114_t *p_dev, uint8_t reg_addr, uint8_t TX_data)
// {
//     if (!p_dev)
//     {
//         return (uint32_t)ERROR_INVALID_PARAM;
//     }

//     return wr_frame(p_dev, reg_addr, &TX_data, 1);
// }

static uint32_t ad4114_write16(ad4114_t *p_dev, uint8_t reg_addr, uint16_t TX_data)
{
    if (!p_dev)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    } 

    uint8_t b[2] = 
    {
        (uint8_t)(TX_data >> 8),
        (uint8_t)TX_data
    };
    return wr_frame(p_dev, reg_addr, b, 2);
}

static uint32_t ad4114_write24(ad4114_t *p_dev, uint8_t reg_addr, uint32_t TX_data)
{
    if (!p_dev)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint8_t b[3] =
    {
        (uint8_t)(TX_data >> 16),
        (uint8_t)(TX_data >> 8),
        (uint8_t)TX_data
    };
    return wr_frame(p_dev, reg_addr, b, 3);
}

__attribute__((unused)) static uint32_t ad4114_read8 (ad4114_t *p_dev, uint8_t reg_addr, uint8_t* p_RX)
{
    if (!p_dev || !p_RX)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }
    
    return rd_frame(p_dev, reg_addr, p_RX, 1);
}

static uint32_t ad4114_read16(ad4114_t *p_dev, uint8_t reg_addr, uint16_t* p_RX)
{
    // PRINTF("\r\n[ad4114_read16] enterred\r\n;");
    if (!p_dev || !p_RX)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint8_t b[2];
    
    uint32_t rc = rd_frame(p_dev, reg_addr, b, 2);

    if (rc != (uint32_t)ERROR_OK)
    {
        return rc;
    }
    
    *p_RX = (uint16_t)((b[0] << 8) | b[1]);
    // PRINTF("\r\n[ad4114_read16] exitted\r\n;");
    return (uint32_t)ERROR_OK;
}

static uint32_t ad4114_read24(ad4114_t *p_dev, uint8_t reg_addr, uint32_t* p_RX)
{
    if (!p_dev || !p_RX)
    {
        return (uint32_t)ERROR_INVALID_PARAM;
    }

    uint8_t b[3];
    
    uint32_t rc = rd_frame(p_dev, reg_addr, b, 3);

    if (rc != (uint32_t)ERROR_OK)
    {
        return rc;
    }
    
    *p_RX = ((uint32_t)b[0] << 16) | ((uint32_t)b[1] << 8) | b[2];
    return (uint32_t)ERROR_OK;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */