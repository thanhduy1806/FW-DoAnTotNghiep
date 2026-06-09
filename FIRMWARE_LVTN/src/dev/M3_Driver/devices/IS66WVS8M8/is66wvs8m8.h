/* 
 * File:   is66wvs4m8.h
 * Author: HTSANG
 *
 * Created on April 3, 2026, 10:18 AM
 */

#ifndef IS66WVS8M8_H
#define	IS66WVS8M8_H

#include "definitions.h"
#include "M3_Driver/components/dio/do.h"
#include "M3_Driver/components/spi/spi_io.h"

typedef struct is66_dev {
    do_t    *cs;
    spi_io_t *spi;
    uint8_t id[8];
} is66_dev_t;

#include "define.h"
/* -------- CMD -------- */
#define IS66WVS8M8_READ_ID_CMD      0x9F
#define IS66WVS8M8_WRITE_CMD        0x02
#define IS66WVS8M8_READ_CMD         0x0B

int is66_write(is66_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);

int is66_read(is66_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);

Std_ReturnType is66_read_id(is66_dev_t *dev);

Std_ReturnType is66_dma_read_id(is66_dev_t *dev);

void SPI1_DMA_TX_Callback(XDMAC_TRANSFER_EVENT event, uintptr_t context);

void SPI1_DMA_RX_Callback(XDMAC_TRANSFER_EVENT event, uintptr_t context);

#endif	/* IS66WVS4M8_H */

