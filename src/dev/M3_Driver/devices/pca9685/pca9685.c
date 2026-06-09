#include "pca9685.h"
#include "define.h"
/* -------- optional delay hook -------- */
__attribute__((weak)) void pca9685_delay_ms(uint32_t ms) { (void)ms; }

/* -------- I2C helpers (trả mã lỗi dương) -------- */
static int wr8(pca9685_t* d, uint8_t reg, uint8_t val) {
    if (!d || !d->bus) return (int)ERROR_INVALID_PARAM;
    uint8_t buf[2] = { reg, val };
    int w = (int)i2c_io_send(d->bus, d->addr7, (const char*)buf, 2);
    return (w == 2) ? (int)ERROR_OK : (int)ERROR_I2C_TIMEOUT;
}
static int rd8(pca9685_t* d, uint8_t reg, uint8_t* val) {
    if (!d || !d->bus || !val) return (int)ERROR_INVALID_PARAM;
    int w = (int)i2c_io_send(d->bus, d->addr7, (const char*)&reg, 1);
    if (w != 1) return (int)ERROR_I2C_TIMEOUT;
    int r = (int)i2c_io_recv(d->bus, d->addr7, (char*)val, 1);
    return (r == 1) ? (int)ERROR_OK : (int)ERROR_I2C_TIMEOUT;
}
static int wr4(pca9685_t* d, uint8_t reg, uint16_t on, uint16_t off) {
    if (!d || !d->bus) return (int)ERROR_INVALID_PARAM;
    uint8_t buf[5];
    buf[0] = reg;
    buf[1] = (uint8_t)(on & 0xFF);
    buf[2] = (uint8_t)(on >> 8);
    buf[3] = (uint8_t)(off & 0xFF);
    buf[4] = (uint8_t)(off >> 8);
    int w = (int)i2c_io_send(d->bus, d->addr7, (const char*)buf, 5);
    return (w == 5) ? (int)ERROR_OK : (int)ERROR_I2C_TIMEOUT;
}

/* -------- Public API -------- */

int pca9685_init_min(pca9685_t* dev, i2c_io_t* bus, uint8_t addr7)
{
    if (!dev || !bus) return (int)ERROR_INVALID_PARAM;
    dev->bus   = bus;
    dev->addr7 = addr7 & 0x7F;

    /* MODE1: bật Auto-Increment + ALLCALL, đảm bảo SLEEP=0 */
    uint8_t m1 = 0;
    (void)rd8(dev, PCA9685_MODE1, &m1); /* nếu lỗi, m1=0 -> dùng mặc định */
    m1 = (uint8_t)((m1 | MODE1_AI | MODE1_ALLCALL) & (uint8_t)~MODE1_SLEEP);
    int rc = wr8(dev, PCA9685_MODE1, m1);
    if (rc != (int)ERROR_OK) return rc;

    /* MODE2: chọn Totem-pole */
    uint8_t m2 = 0;
    (void)rd8(dev, PCA9685_MODE2, &m2);
    m2 = (uint8_t)((m2 | MODE2_OUTDRV) & (uint8_t)~MODE2_INVRT);
    rc = wr8(dev, PCA9685_MODE2, m2);
    if (rc != (int)ERROR_OK) return rc;

    /* Clear all channels */
    rc = wr4(dev, PCA9685_ALL_LED_ON_L, 0, 0);
    if (rc != (int)ERROR_OK) return rc;

    /* Soft restart (khuyến nghị) */
    (void)wr8(dev, PCA9685_MODE1, (uint8_t)(m1 | MODE1_RESTART));
    pca9685_delay_ms(2);
    return (int)ERROR_OK;
}

int pca9685_set_freq_hz(pca9685_t* dev, uint16_t freq_hz)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    if (freq_hz < 24)   freq_hz = 24;
    if (freq_hz > 1526) freq_hz = 1526;

    /* prescale = round( 25MHz / (4096 * freq) - 1 ) */
    float prescale_f = (25000000.0f / 4096.0f) / (float)freq_hz - 1.0f;
    uint8_t prescale = (uint8_t)(prescale_f + 0.5f);

    uint8_t oldmode = 0;
    int rc = rd8(dev, PCA9685_MODE1, &oldmode);
    if (rc != (int)ERROR_OK) return rc;

    /* Vào SLEEP để ghi PRESCALE */
    uint8_t sleepmode = (uint8_t)((oldmode & 0x7F) | MODE1_SLEEP);
    rc = wr8(dev, PCA9685_MODE1, sleepmode);
    if (rc != (int)ERROR_OK) return rc;
    pca9685_delay_ms(2);

    rc = wr8(dev, PCA9685_PRESCALE, prescale);
    if (rc != (int)ERROR_OK) return rc;

    /* Thoát SLEEP, giữ AI|ALLCALL */
    rc = wr8(dev, PCA9685_MODE1, (uint8_t)((oldmode | MODE1_AI | MODE1_ALLCALL) & (uint8_t)~MODE1_SLEEP));
    if (rc != (int)ERROR_OK) return rc;
    pca9685_delay_ms(2);

    /* RESTART */
    rc = wr8(dev, PCA9685_MODE1, (uint8_t)((oldmode | MODE1_AI | MODE1_ALLCALL | MODE1_RESTART) & 0xFF));
    if (rc != (int)ERROR_OK) return rc;

    return (int)ERROR_OK;
}

int pca9685_set_duty_permille(pca9685_t* dev, uint8_t ch, uint16_t duty_permil)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    if (ch > 15) return (int)ERROR_INVALID_PARAM;
    if (duty_permil > 1000) duty_permil = 1000;

    uint8_t base = (uint8_t)(PCA9685_LED0_ON_L + 4u * ch);

    if (duty_permil == 0) {
        /* FULL OFF kênh: OFF_H |= 0x10; đồng thời clear ON_H bit FULL ON */
        uint8_t on_h = 0, off_h = 0;
        int rc = rd8(dev, (uint8_t)(base + 1), &on_h);  if (rc != (int)ERROR_OK) return rc;
        rc = rd8(dev, (uint8_t)(base + 3), &off_h);     if (rc != (int)ERROR_OK) return rc;
        on_h  &= (uint8_t)~0x10;
        off_h |=  0x10;
        rc = wr8(dev, (uint8_t)(base + 1), on_h);       if (rc != (int)ERROR_OK) return rc;
        rc = wr8(dev, (uint8_t)(base + 3), off_h);      if (rc != (int)ERROR_OK) return rc;
        return (int)ERROR_OK;
    }

    if (duty_permil == 1000) {
        /* FULL ON kênh: ON_H |= 0x10; OFF_H &= ~0x10 */
        uint8_t on_h = 0, off_h = 0;
        int rc = rd8(dev, (uint8_t)(base + 1), &on_h);  if (rc != (int)ERROR_OK) return rc;
        rc = rd8(dev, (uint8_t)(base + 3), &off_h);     if (rc != (int)ERROR_OK) return rc;
        on_h  |=  0x10;
        off_h &= (uint8_t)~0x10;
        rc = wr8(dev, (uint8_t)(base + 1), on_h);       if (rc != (int)ERROR_OK) return rc;
        rc = wr8(dev, (uint8_t)(base + 3), off_h);      if (rc != (int)ERROR_OK) return rc;
        return (int)ERROR_OK;
    }

    /* PWM bình thường: ON=0, OFF=counts (0..4095), clear bit FULL ON/OFF */
    uint32_t counts = (uint32_t)4096u * (uint32_t)duty_permil;
    counts = (counts + 500u) / 1000u;   /* làm tròn */
    if (counts >= 4096u) counts = 4095u;

    /* Clear bits FULL ON/OFF trước khi ghi giá trị */
    uint8_t on_h = 0, off_h = 0;
    int rc = rd8(dev, (uint8_t)(base + 1), &on_h);  if (rc != (int)ERROR_OK) return rc;
    rc = rd8(dev, (uint8_t)(base + 3), &off_h);     if (rc != (int)ERROR_OK) return rc;
    on_h  &= (uint8_t)~0x10;
    off_h &= (uint8_t)~0x10;
    rc = wr8(dev, (uint8_t)(base + 1), on_h);       if (rc != (int)ERROR_OK) return rc;
    rc = wr8(dev, (uint8_t)(base + 3), off_h);      if (rc != (int)ERROR_OK) return rc;

    return wr4(dev, base, 0, (uint16_t)counts);
}

/* ---------------- Output enable/disable toàn bộ ---------------- */

int pca9685_output_disable(pca9685_t* dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    /* Set FULL OFF cho ALL_LED: ALL_LED_OFF_H |= 0x10 */
    uint8_t buf[5] = { PCA9685_ALL_LED_OFF_L, 0x00, 0x00, 0x00, 0x10 };
    int w = (int)i2c_io_send(dev->bus, dev->addr7, (const char*)buf, 5);
    return (w == 5) ? (int)ERROR_OK : (int)ERROR_I2C_TIMEOUT;
}

int pca9685_output_enable(pca9685_t* dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    /* Clear FULL OFF cho ALL_LED: ALL_LED_OFF_H &= ~0x10 */
    uint8_t buf[5] = { PCA9685_ALL_LED_OFF_L, 0x00, 0x00, 0x00, 0x00 };
    int w = (int)i2c_io_send(dev->bus, dev->addr7, (const char*)buf, 5);
    return (w == 5) ? (int)ERROR_OK : (int)ERROR_I2C_TIMEOUT;
}

/* ---------------- Bật/Tắt từng kênh ---------------- */

int pca9685_channel_disable(pca9685_t* dev, uint8_t ch)
{
    if (!dev || ch > 15) return (int)ERROR_INVALID_PARAM;
    uint8_t base = (uint8_t)(PCA9685_LED0_ON_L + 4u * ch);

    /* Clear FULL ON, Set FULL OFF */
    uint8_t on_h = 0, off_h = 0;
    int rc = rd8(dev, (uint8_t)(base + 1), &on_h);  if (rc != (int)ERROR_OK) return rc; /* ON_H */
    rc = rd8(dev, (uint8_t)(base + 3), &off_h);     if (rc != (int)ERROR_OK) return rc; /* OFF_H */
    on_h  &= (uint8_t)~0x10;
    off_h |=  0x10;
    rc = wr8(dev, (uint8_t)(base + 1), on_h);       if (rc != (int)ERROR_OK) return rc;
    rc = wr8(dev, (uint8_t)(base + 3), off_h);      if (rc != (int)ERROR_OK) return rc;
    return (int)ERROR_OK;
}

int pca9685_channel_enable(pca9685_t* dev, uint8_t ch)
{
    if (!dev || ch > 15) return (int)ERROR_INVALID_PARAM;
    uint8_t base = (uint8_t)(PCA9685_LED0_ON_L + 4u * ch);

    /* Clear FULL ON và FULL OFF -> PWM theo duty đã cài */
    uint8_t on_h = 0, off_h = 0;
    int rc = rd8(dev, (uint8_t)(base + 1), &on_h);  if (rc != (int)ERROR_OK) return rc;
    rc = rd8(dev, (uint8_t)(base + 3), &off_h);     if (rc != (int)ERROR_OK) return rc;
    on_h  &= (uint8_t)~0x10;
    off_h &= (uint8_t)~0x10;
    rc = wr8(dev, (uint8_t)(base + 1), on_h);       if (rc != (int)ERROR_OK) return rc;
    rc = wr8(dev, (uint8_t)(base + 3), off_h);      if (rc != (int)ERROR_OK) return rc;
    return (int)ERROR_OK;
}

int pca9685_channel_full_on(pca9685_t* dev, uint8_t ch)
{
    if (!dev || ch > 15) return (int)ERROR_INVALID_PARAM;
    uint8_t base = (uint8_t)(PCA9685_LED0_ON_L + 4u * ch);

    /* Set FULL ON, Clear FULL OFF */
    uint8_t on_h = 0, off_h = 0;
    int rc = rd8(dev, (uint8_t)(base + 1), &on_h);  if (rc != (int)ERROR_OK) return rc;
    rc = rd8(dev, (uint8_t)(base + 3), &off_h);     if (rc != (int)ERROR_OK) return rc;
    on_h  |=  0x10;
    off_h &= (uint8_t)~0x10;
    rc = wr8(dev, (uint8_t)(base + 1), on_h);       if (rc != (int)ERROR_OK) return rc;
    rc = wr8(dev, (uint8_t)(base + 3), off_h);      if (rc != (int)ERROR_OK) return rc;
    return (int)ERROR_OK;
}

int pca9685_channel_full_off(pca9685_t* dev, uint8_t ch)
{
    /* Alias cho disable */
    return pca9685_channel_disable(dev, ch);
}
