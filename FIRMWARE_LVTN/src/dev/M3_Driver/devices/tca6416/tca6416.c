#include "tca6416.h"

/* ---------- Internal helpers ---------- */

static int write_reg16(tca6416a_t* dev, uint8_t reg, uint16_t v)
{
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = (uint8_t)(v & 0xFF);
    buf[2] = (uint8_t)(v >> 8);
    int w = (int)i2c_io_send(dev->bus, dev->addr7, (const char*)buf, 3);
    return (w == 0) ? 0 : 1;
}

static int read_reg16(tca6416a_t* dev, uint8_t reg, uint16_t* out)
{
    /* Ghi con trỏ thanh ghi, sau đó đọc 2 byte */
    uint8_t r = reg;
    int w = (int)i2c_io_send(dev->bus, dev->addr7, (const char*)&r, 1);
    if (w != 0) return 1;

    uint8_t b[2] = {0,0};
    int rcv = (int)i2c_io_recv(dev->bus, dev->addr7, (char*)b, 2);
    if (rcv != 0) return 2;

    *out = (uint16_t)b[0] | ((uint16_t)b[1] << 8);
    return 0;
}

/* ---------- Public API ---------- */

int tca6416a_init(tca6416a_t* dev, i2c_io_t* bus, uint8_t addr7)
{
    if (!dev || !bus) return -1;
    dev->bus      = bus;
    dev->addr7    = addr7 & 0x7F;
    dev->cache_out = 0x0000;
    dev->cache_cfg = 0xFFFF; /* power-on mặc định: tất cả là input */

    /* Probe bằng cách đọc INPUT */
    uint16_t tmp = 0;
    if (read_reg16(dev, TCA6416A_REG_INPUT0, &tmp) < 0)
        return 2;

    /* Đồng bộ cache CONFIG & OUTPUT thực tế */
    if (read_reg16(dev, TCA6416A_REG_CFG0, &dev->cache_cfg) < 0)
        return 3;

    /* Đọc input hiện tại rồi “trộn” vào cache_out cho các bit input */
    uint16_t in_now = 0;
    if (read_reg16(dev, TCA6416A_REG_INPUT0, &in_now) < 0)
        return 4;
    dev->cache_out = (dev->cache_out & (uint16_t)~dev->cache_cfg) | (in_now & dev->cache_cfg);

    return 0;
}

int tca6416a_read_inputs(tca6416a_t* dev, uint16_t* in16)
{
    if (!dev || !in16) return -1;
    uint16_t v = 0;
    int rc = read_reg16(dev, TCA6416A_REG_INPUT0, &v);
    if (rc > 0) return rc;

    /* Giữ cache_out cho các bit output, cập nhật các bit input từ phần cứng */
    dev->cache_out = (dev->cache_out & (uint16_t)~dev->cache_cfg) | (v & dev->cache_cfg);
    *in16 = v;
    return 0;
}

int tca6416a_write_outputs(tca6416a_t* dev, uint16_t out16)
{
    if (!dev) return 1;
    int rc = write_reg16(dev, TCA6416A_REG_OUTPUT0, out16);
    if (rc > 0) return rc;
    dev->cache_out = out16;
    return 0;
}

int tca6416a_read_outputs(tca6416a_t* dev, uint16_t* out16)
{
    if (!dev || !out16) return -1;
    /* OUTPUT register có thể đọc lại, nhưng đọc cache đủ nhanh */
    *out16 = dev->cache_out;
    return 0;
}

int tca6416a_write_modes(tca6416a_t* dev, uint16_t mode16)
{
    if (!dev) return -1;
    int rc = write_reg16(dev, TCA6416A_REG_CFG0, mode16);
    if (rc > 0) return rc;
    dev->cache_cfg = mode16;
    return 0;
}

int tca6416a_read_modes(tca6416a_t* dev, uint16_t* mode16)
{
    if (!dev || !mode16) return -1;
    uint16_t v = 0;
    int rc = read_reg16(dev, TCA6416A_REG_CFG0, &v);
    if (rc > 0) return rc;
    dev->cache_cfg = v;
    *mode16 = v;
    return 0;
}

int tca6416a_pin_mode(tca6416a_t* dev, uint8_t pin, tca6416a_mode_t mode)
{
    if (!dev || pin > 15) return -1;
    uint16_t cfg = dev->cache_cfg;
    if (mode == TCA6416A_MODE_INPUT)  cfg |=  BIT(pin);
    else                               cfg &= (uint16_t)~BIT(pin);
    return tca6416a_write_modes(dev, cfg);
}

int tca6416a_pin_write(tca6416a_t* dev, uint8_t pin, uint8_t level)
{
    if (!dev || pin > 15) return -1;
    /* Không tự động đổi hướng: giả định pin đã đặt OUTPUT từ trước */
    uint16_t out = dev->cache_out;
    if (level) out |=  BIT(pin);
    else       out &= (uint16_t)~BIT(pin);
    return tca6416a_write_outputs(dev, out);
}

int tca6416a_pin_read(tca6416a_t* dev, uint8_t pin)
{
    if (!dev || pin > 15) return -1;
    uint16_t v = 0;
    int rc = tca6416a_read_inputs(dev, &v);
    if (rc > 0) return rc;
    return (v & BIT(pin)) ? 1 : 0;
}

int tca6416a_write_polarity(tca6416a_t* dev, uint16_t pol16)
{
    if (!dev) return -1;
    return write_reg16(dev, TCA6416A_REG_POL0, pol16);
}

int tca6416a_read_polarity(tca6416a_t* dev, uint16_t* pol16)
{
    if (!dev || !pol16) return -1;
    return read_reg16(dev, TCA6416A_REG_POL0, pol16);
}
