#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "os_hal.h"
#include "samv71q21b.h"
#include "spi_io.h"
#include "define.h"

#define SPI_TIMEOUT_MS      100U
#define SPI_MAX_BUS_NUMBER  2U

static spi_registers_t *const spi_periph[SPI_MAX_BUS_NUMBER + 1U] = {
    NULL,
    SPI0_REGS,
    SPI1_REGS
};

static const uint32_t spi_clock_en[SPI_MAX_BUS_NUMBER + 1U] = {
    0U,
    ID_SPI0,
    ID_SPI1
};

/*==============================================================================
 * Private Functions
 *============================================================================*/

static spi_registers_t *spi_get_instance(uint32_t spi_port) {
    if ((spi_port == 0U) || (spi_port > SPI_MAX_BUS_NUMBER)) {
        return NULL;
    }

    return spi_periph[spi_port];
}

static void spi_enable_clock(uint32_t spi_port) {
    if ((spi_port == 0U) || (spi_port > SPI_MAX_BUS_NUMBER)) {
        return;
    }

    uint32_t id = spi_clock_en[spi_port];

    if (id < 32U) {
        PMC_REGS->PMC_PCER0 = (1UL << id);
    } else {
        PMC_REGS->PMC_PCER1 = (1UL << (id - 32U));
    }
}

/*==============================================================================
 * Synchronous Transfer
 *============================================================================*/

uint32_t spi_io_transfer_sync(spi_io_t *me,
                              const uint8_t *pui8TxBuff,
                              uint8_t *pui8RxBuff,
                              uint32_t ui32Length) {
    if (me == NULL) {
        return ERROR_INVALID_PARAM;
    }

    if (ui32Length == 0U) {
        return ERROR_OK;
    }

    spi_registers_t *spi = spi_get_instance(me->ui32SpiPort);

    if (spi == NULL) {
        return ERROR_INVALID_PARAM;
    }

    for (uint32_t i = 0U; i < ui32Length; i++) {
        /* Wait TX ready */
        if (os_reg_wait_flag_blocking(&spi->SPI_SR,
                                      SPI_SR_TDRE_Msk,
                                      SPI_TIMEOUT_MS) != ERROR_OK) {
            return ERROR_TIMEOUT;
        }

        uint8_t tx_data = (pui8TxBuff != NULL) ? pui8TxBuff[i] : 0xFFU;

        /* Write transmit data */
        spi->SPI_TDR = SPI_TDR_TD(tx_data);

        /* Wait RX ready */
        if (os_reg_wait_flag_blocking(&spi->SPI_SR,
                                      SPI_SR_RDRF_Msk,
                                      SPI_TIMEOUT_MS) != ERROR_OK) {
            return ERROR_TIMEOUT;
        }

        /* Check SPI error */
        uint32_t sr = spi->SPI_SR;

        if ((sr & (SPI_SR_OVRES_Msk | SPI_SR_MODF_Msk)) != 0U) {
            (void)spi->SPI_RDR;
            return ERROR_SPI;
        }

        uint8_t rx_data = (uint8_t)(spi->SPI_RDR & SPI_RDR_RD_Msk);

        if (pui8RxBuff != NULL) {
            pui8RxBuff[i] = rx_data;
        }
    }

    /* Wait transfer complete */
    if (os_reg_wait_flag_blocking(&spi->SPI_SR,
                                  SPI_SR_TXEMPTY_Msk,
                                  SPI_TIMEOUT_MS) != ERROR_OK) {
        return ERROR_TIMEOUT;
    }

    return ERROR_OK;
}

/*==============================================================================
 * Read / Write
 *============================================================================*/

uint32_t spi_io_read_sync(spi_io_t *me,
                          uint8_t *pui8RxBuff,
                          uint32_t ui32Length) {
    if ((me == NULL) || (pui8RxBuff == NULL)) {
        return ERROR_INVALID_PARAM;
    }

    return spi_io_transfer_sync(me, NULL, pui8RxBuff, ui32Length);
}

uint32_t spi_io_write_sync(spi_io_t *me,
                           const uint8_t *pui8TxBuff,
                           uint32_t ui32Length) {
    if ((me == NULL) || (pui8TxBuff == NULL)) {
        return ERROR_INVALID_PARAM;
    }

    return spi_io_transfer_sync(me, pui8TxBuff, NULL, ui32Length);
}

/*==============================================================================
 * Configuration
 *============================================================================*/

uint32_t spi_io_set_frame_size(spi_io_t *me, uint8_t bits) {
    if (me == NULL) {
        return ERROR_INVALID_PARAM;
    }

    spi_registers_t *spi = spi_get_instance(me->ui32SpiPort);

    if (spi == NULL) {
        return ERROR_INVALID_PARAM;
    }

    uint32_t csr = spi->SPI_CSR[3];

    csr &= ~SPI_CSR_BITS_Msk;

    switch (bits) {
        case 8:
            csr |= SPI_CSR_BITS_8_BIT;
            break;

        case 16:
            csr |= SPI_CSR_BITS_16_BIT;
            break;

        default:
            return ERROR_INVALID_PARAM;
    }

    spi->SPI_CSR[3] = csr;

    return ERROR_OK;
}

uint32_t spi_io_set_mode(spi_io_t *me, uint8_t mode) {
    if (me == NULL) {
        return ERROR_INVALID_PARAM;
    }

    spi_registers_t *spi = spi_get_instance(me->ui32SpiPort);

    if (spi == NULL) {
        return ERROR_INVALID_PARAM;
    }

    uint32_t csr = spi->SPI_CSR[3];

    csr &= ~(SPI_CSR_CPOL_Msk | SPI_CSR_NCPHA_Msk);

    switch (mode) {
        case 0:
            csr |= SPI_CSR_CPOL_IDLE_LOW |
                   SPI_CSR_NCPHA_VALID_LEADING_EDGE;
            break;

        case 1:
            csr |= SPI_CSR_CPOL_IDLE_LOW |
                   SPI_CSR_NCPHA_VALID_TRAILING_EDGE;
            break;

        case 2:
            csr |= SPI_CSR_CPOL_IDLE_HIGH |
                   SPI_CSR_NCPHA_VALID_LEADING_EDGE;
            break;

        case 3:
            csr |= SPI_CSR_CPOL_IDLE_HIGH |
                   SPI_CSR_NCPHA_VALID_TRAILING_EDGE;
            break;

        default:
            return ERROR_INVALID_PARAM;
    }

    spi->SPI_CR = SPI_CR_SPIDIS_Msk;
    spi->SPI_CSR[3] = csr;
    spi->SPI_CR = SPI_CR_SPIEN_Msk;

    return ERROR_OK;
}

uint32_t spi_io_set_prescale(spi_io_t *me, uint8_t scbr) {
    if ((me == NULL) || (scbr == 0U)) {
        return ERROR_INVALID_PARAM;
    }

    spi_registers_t *spi = spi_get_instance(me->ui32SpiPort);

    if (spi == NULL) {
        return ERROR_INVALID_PARAM;
    }

    uint32_t csr = spi->SPI_CSR[3];

    csr &= ~SPI_CSR_SCBR_Msk;
    csr |= SPI_CSR_SCBR(scbr);

    spi->SPI_CR = SPI_CR_SPIDIS_Msk;
    spi->SPI_CSR[3] = csr;
    spi->SPI_CR = SPI_CR_SPIEN_Msk;

    return ERROR_OK;
}