#include "mb85rs2mt.h"
#include "bsp_core.h"
#include <string.h>
#include <stdio.h>
#include "../Config/default/peripheral/usart/plib_usart2_spi.h"
#include "../Config/default/peripheral/xdmac/plib_xdmac.h"

#define FRAM_SPI_BUFFER_SIZE     (1024 + 4)

static inline void mb85rs2mt_select(mb85rs2mt_dev_t *dev) {
    PIOD_REGS->PIO_CODR = (1U << dev->cs->pin);
}

static inline void mb85rs2mt_deselect(mb85rs2mt_dev_t *dev) {
    PIOD_REGS->PIO_SODR = (1U << dev->cs->pin);
}

/* -------- Core Commands -------- */
static int mb85rs2mt_write_enable(mb85rs2mt_dev_t *dev) {
    uint8_t cmd = MB85RS2MT_CMD_WREN;
    mb85rs2mt_select(dev);
    USART2_SPI_WriteRead(&cmd, 1, NULL, 0);
    while (USART2_SPI_IsTransmitterBusy());
    mb85rs2mt_deselect(dev);
}

/**
 */
int mb85rs2mt_write(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length) {
    if (!dev || !buffer || length == 0) return ERROR_INVALID_PARAM;
    mb85rs2mt_write_enable(dev);

    uint8_t spi_tx[length + 4];

    spi_tx[0] = MB85RS2MT_CMD_WRITE;
    spi_tx[1] = (uint8_t) (address >> 16);
    spi_tx[2] = (uint8_t) (address >> 8);
    spi_tx[3] = (uint8_t) (address);
    memcpy(&spi_tx[4], buffer, length);

    mb85rs2mt_select(dev);
    bool ret = USART2_SPI_WriteRead(spi_tx, length + 4, NULL, 0);
    while (USART2_SPI_IsTransmitterBusy());
    mb85rs2mt_deselect(dev);

    if (ret) {
        return ERROR_OK;
    }
    
    return ERROR_FAIL;
}

/**
 */
int mb85rs2mt_read(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length) {
    if (!dev || !buffer || length == 0) return ERROR_INVALID_PARAM;

    uint8_t spi_tx[length + 4];
    uint8_t spi_rx[length + 4];

    spi_tx[0] = MB85RS2MT_CMD_READ;
    spi_tx[1] = (uint8_t) (address >> 16);
    spi_tx[2] = (uint8_t) (address >> 8);
    spi_tx[3] = (uint8_t) (address);

    memset(&spi_tx[4], 0, length);
    uint16_t total_len = 4 + length;

    mb85rs2mt_select(dev);
    bool ret = USART2_SPI_WriteRead(spi_tx, total_len, spi_rx, total_len);
    while (USART2_SPI_IsTransmitterBusy());
    mb85rs2mt_deselect(dev);

    if (ret) {
        memcpy(buffer, &spi_rx[4], length);
        return ERROR_OK;
    }
    
    return ERROR_FAIL;
}