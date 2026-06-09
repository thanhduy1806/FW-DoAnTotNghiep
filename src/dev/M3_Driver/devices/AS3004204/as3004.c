#include "as3004.h"
#include <string.h>

/* ========================================================================== */
/*                          Low-level SPI helpers                             */
/* ========================================================================== */

static inline void cs_active(as3004_t *d) { do_reset(&d->cs); }
static inline void cs_idle  (as3004_t *d) { do_set(&d->cs);   }

/* Gửi 1 khung SPI: tx->rx, độ dài n byte, giữ CS trong suốt giao dịch */
static int spi_xfer(as3004_t *d, const uint8_t *tx, uint8_t *rx, size_t n)
{
    if (!d || !d->spi || n == 0) return (int)ERROR_INVALID_PARAM;
    uint32_t st = spi_io_transfer_sync(d->spi, (uint8_t*)tx, rx, (uint32_t)n);
    return (st == ERROR_OK) ? (int)ERROR_OK : (int)st;
}

/* Gửi 1 byte lệnh */
static int send_cmd(as3004_t *d, uint8_t cmd)
{
    uint8_t rx;
    cs_active(d);
    int rc = spi_xfer(d, &cmd, &rx, 1);
    cs_idle(d);
    return rc;
}

/* Gửi lệnh + địa chỉ 24-bit (MSB trước), giữ CS, KHÔNG nhả CS */
static int send_cmd_addr_hold(as3004_t *d, uint8_t cmd, uint32_t addr)
{
    uint8_t hdr[4] = { cmd, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr };
    uint8_t rxd[4];
    cs_active(d);
    int rc = spi_xfer(d, hdr, rxd, sizeof hdr);
    if (rc != (int)ERROR_OK) { cs_idle(d); return rc; }
    return (int)ERROR_OK;
}

/* Một số driver SPI cần TX dummy khi đọc: dùng 0xFF */
static int recv_data_hold_dummy(as3004_t *d, uint8_t *buf, size_t len)
{
    uint8_t dummy_tx[32];
    size_t  left = len;
    while (left) {
        size_t chunk = (left > sizeof(dummy_tx)) ? sizeof(dummy_tx) : left;
        memset(dummy_tx, 0xFF, chunk);
        int rc = spi_xfer(d, dummy_tx, buf + (len - left), chunk);
        if (rc != (int)ERROR_OK) return rc;
        left -= chunk;
    }
    return (int)ERROR_OK;
}

static int send_data_hold(as3004_t *d, const uint8_t *buf, size_t len)
{
    return spi_xfer(d, buf, NULL, len);
}

/* ========================================================================== */
/*                                Public API                                  */
/* ========================================================================== */

int as3004_init(as3004_t *dev, spi_io_t *spi, const do_t *cs)
{
    if (!dev || !spi || !cs) return (int)ERROR_INVALID_PARAM;
    dev->spi = spi;
    dev->cs  = *cs;
    dev->dummy_cycles = 1; /* 1 byte dummy mặc định cho FAST READ */

    /* CS idle high */
    cs_idle(dev);
    return (int)ERROR_OK;
}

int as3004_write_enable(as3004_t *dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    return send_cmd(dev, AS3004_CMD_WREN);
}

int as3004_write_disable(as3004_t *dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    return send_cmd(dev, AS3004_CMD_WRDI);
}

int as3004_read_id(as3004_t *dev, uint8_t jedec_id[3])
{
    if (!dev || !jedec_id) return (int)ERROR_INVALID_PARAM;

    uint8_t cmd = AS3004_CMD_READ_ID;
    uint8_t rx;
    cs_active(dev);
    int rc = spi_xfer(dev, &cmd, &rx, 1);
    if (rc != (int)ERROR_OK) { cs_idle(dev); return rc; }

    rc = recv_data_hold_dummy(dev, jedec_id, 3);
    cs_idle(dev);
    return rc;
}

int as3004_read_status(as3004_t *dev, uint8_t *sr)
{
    if (!dev || !sr) return (int)ERROR_INVALID_PARAM;

    uint8_t cmd = AS3004_CMD_RDSR, rx;
    cs_active(dev);
    int rc = spi_xfer(dev, &cmd, &rx, 1);
    if (rc != (int)ERROR_OK) { cs_idle(dev); return rc; }

    rc = recv_data_hold_dummy(dev, sr, 1);
    cs_idle(dev);
    return rc;
}

int as3004_write_status(as3004_t *dev, uint8_t sr)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;

    int rc = as3004_write_enable(dev);
    if (rc != (int)ERROR_OK) return rc;

    uint8_t buf[2] = { AS3004_CMD_WRSR, sr };
    cs_active(dev);
    rc = spi_xfer(dev, buf, NULL, sizeof buf);
    cs_idle(dev);
    return rc;
}

int as3004_read(as3004_t *dev, uint32_t addr, uint8_t *buf, size_t len)
{
    if (!dev || !buf || len == 0) return (int)ERROR_INVALID_PARAM;
    if (addr > AS3004_ADDR_MAX || (addr + len - 1u) > AS3004_ADDR_MAX) return (int)ERROR_INVALID_PARAM;

    int rc = send_cmd_addr_hold(dev, AS3004_CMD_READ, addr);
    if (rc != (int)ERROR_OK) return rc;

    rc = recv_data_hold_dummy(dev, buf, len);
    cs_idle(dev);
    return rc;
}

int as3004_read_fast(as3004_t *dev, uint32_t addr, uint8_t *buf, size_t len)
{
    if (!dev || !buf || len == 0) return (int)ERROR_INVALID_PARAM;
    if (addr > AS3004_ADDR_MAX || (addr + len - 1u) > AS3004_ADDR_MAX) return (int)ERROR_INVALID_PARAM;

    int rc = send_cmd_addr_hold(dev, AS3004_CMD_FAST_READ, addr);
    if (rc != (int)ERROR_OK) return rc;

    /* Dummy bytes */
    uint8_t dummy[8], rx[8];
    uint8_t n = dev->dummy_cycles; if (n > sizeof(dummy)) n = (uint8_t)sizeof(dummy);
    memset(dummy, 0xFF, n);
    rc = spi_xfer(dev, dummy, rx, n);
    if (rc != (int)ERROR_OK) { cs_idle(dev); return rc; }

    rc = recv_data_hold_dummy(dev, buf, len);
    cs_idle(dev);
    return rc;
}

int as3004_write(as3004_t *dev, uint32_t addr, const uint8_t *buf, size_t len)
{
    if (!dev || !buf || len == 0) return (int)ERROR_INVALID_PARAM;
    if (addr > AS3004_ADDR_MAX || (addr + len - 1u) > AS3004_ADDR_MAX) return (int)ERROR_INVALID_PARAM;

    int rc = as3004_write_enable(dev);
    if (rc != (int)ERROR_OK) return rc;

    rc = send_cmd_addr_hold(dev, AS3004_CMD_WRITE, addr);
    if (rc != (int)ERROR_OK) return rc;

    rc = send_data_hold(dev, buf, len);
    cs_idle(dev);

    (void)as3004_write_disable(dev);
    return rc;
}

/* ------------------ Volatile / Non-Volatile Configuration ------------------ */

static int _cfg_read(as3004_t *dev, uint8_t cmd, uint8_t index, uint8_t *buf, size_t len)
{
    if (!dev || !buf || len == 0) return (int)ERROR_INVALID_PARAM;

    uint8_t hdr[2] = { cmd, index };
    uint8_t rx[2];
    cs_active(dev);
    int rc = spi_xfer(dev, hdr, rx, sizeof hdr);
    if (rc != (int)ERROR_OK) { cs_idle(dev); return rc; }

    rc = recv_data_hold_dummy(dev, buf, len);
    cs_idle(dev);
    return rc;
}

static int _cfg_write(as3004_t *dev, uint8_t cmd, uint8_t index, const uint8_t *buf, size_t len)
{
    if (!dev || !buf || len == 0) return (int)ERROR_INVALID_PARAM;

    int rc = as3004_write_enable(dev);
    if (rc != (int)ERROR_OK) return rc;

    uint8_t hdr[2] = { cmd, index };
    uint8_t rx[2];
    cs_active(dev);
    rc = spi_xfer(dev, hdr, rx, sizeof hdr);
    if (rc == (int)ERROR_OK) rc = send_data_hold(dev, buf, len);
    cs_idle(dev);

    (void)as3004_write_disable(dev);
    return rc;
}

int as3004_read_vol (as3004_t *dev, uint8_t index, uint8_t *buf, size_t len) { return _cfg_read (dev, AS3004_CMD_RD_VOL,  index, buf, len); }
int as3004_write_vol(as3004_t *dev, uint8_t index, const uint8_t *buf, size_t len) { return _cfg_write(dev, AS3004_CMD_WR_VOL,  index, buf, len); }
int as3004_read_nvol (as3004_t *dev, uint8_t index, uint8_t *buf, size_t len) { return _cfg_read (dev, AS3004_CMD_RD_NVOL, index, buf, len); }
int as3004_write_nvol(as3004_t *dev, uint8_t index, const uint8_t *buf, size_t len) { return _cfg_write(dev, AS3004_CMD_WR_NVOL, index, buf, len); }

/* ----------------------------------- Flags --------------------------------- */

int as3004_read_flags(as3004_t *dev, uint8_t *flags)
{
    if (!dev || !flags) return (int)ERROR_INVALID_PARAM;

    uint8_t cmd = AS3004_CMD_RD_FLAGS, rx;
    cs_active(dev);
    int rc = spi_xfer(dev, &cmd, &rx, 1);
    if (rc != (int)ERROR_OK) { cs_idle(dev); return rc; }

    rc = recv_data_hold_dummy(dev, flags, 1);
    cs_idle(dev);
    return rc;
}

int as3004_clear_flags(as3004_t *dev)
{
    if (!dev) return (int)ERROR_INVALID_PARAM;
    return send_cmd(dev, AS3004_CMD_CLR_FLAGS);
}
