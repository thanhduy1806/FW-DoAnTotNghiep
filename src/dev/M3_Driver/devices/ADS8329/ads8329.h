/* 
 * File:   ads8329.h
 * Author: HTSANG
 *
 * Created on March 19, 2026, 1:37 PM
 */

#ifndef ADS8329_H
#define	ADS8329_H

#include "M3_Driver/components/dio/do.h"
#include "M3_Driver/components/spi/spi_io.h"

#define ADS8329_STATE_IDLE        0
#define ADS8329_STATE_CONVERTING  1
#define ADS8329_STATE_READING     2
#define ADS8329_STATE_DONE        3

typedef struct {
    spi_io_t   *spi;                                    /* SPI bus */
    do_t       *cs;                                     /* CS pin (Digital Output) */
    do_t       *conv;                                   /* CONV pin (Digital Output) */
    do_t       *eoc;                                    /* OEC pin (Digital Input) */
    uint16_t    raw; 
    uint8_t     state;
} ads8329_dev_t;

void ads8329_init(ads8329_dev_t *dev);
void ads8329_trigge_conv(ads8329_dev_t *dev);
void ads8329_spi_read_val(ads8329_dev_t *dev);
void ads8329_spi_irq_callback(ads8329_dev_t *dev);

#endif	/* ADS8329_H */

