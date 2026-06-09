#include "ads8329.h"
#include "board_v71_satellite.h"
#include "samv71q21b.h"
#include "../src/packs/ATSAMV71Q21B_DFP/component/spi.h"

void ads8329_init(ads8329_dev_t *dev) {
    return;
}

/*
 conv pin is IO_output
 */
void ads8329_trigge_conv(ads8329_dev_t *dev) {
    dev->state = ADS8329_STATE_CONVERTING;

    PIOA_REGS->PIO_CODR = (uint32_t) 1U << PHOTO_ADC_CONV_PIN; // CONV Low
    PIOD_REGS->PIO_SODR = (uint32_t) 1U << PHOTO_ADC_CS_PIN; // CS High
    PIOA_REGS->PIO_SODR = (uint32_t) 1U << PHOTO_ADC_CONV_PIN; // CONV High
}

void ads8329_spi_read_val(ads8329_dev_t *dev) {
    /* tranh chong SPI */
    if (!(SPI0_REGS->SPI_SR & SPI_SR_TXEMPTY_Msk))
        return;

    /* set 16-bit frame cho ADS8329 */
    // spi_io_set_frame_size(dev->spi, 16);

    dev->state = ADS8329_STATE_READING;

    /* CS Low */
    PIOD_REGS->PIO_CODR = (1U << PHOTO_ADC_CS_PIN);

//    /* Clear toan bo RX FIFO */
//    while (SPI0_REGS->SPI_SR & SPI_SR_RDRF_Msk) {
//        volatile uint32_t dummy = SPI0_REGS->SPI_RDR;
//        (void) dummy;
//    }

    /* Enable RX interrupt */
    SPI0_REGS->SPI_IER = SPI_IER_RDRF_Msk;

    /* Generate 16 clocks */
    SPI0_REGS->SPI_TDR = 0xAAAA;
}

void ads8329_spi_irq_callback(ads8329_dev_t *dev) {
    /* Check dung state */
    if (dev->state != ADS8329_STATE_READING)
        return;

    uint16_t rx = 0;
    /* Dam bao co data */
    if (SPI0_REGS->SPI_SR & SPI_SR_RDRF_Msk)
        rx = (uint16_t) (SPI0_REGS->SPI_RDR & 0xFFFF);
    else
        return;

    /* Neu con data trong FIFO thi clear het */
    while (SPI0_REGS->SPI_SR & SPI_SR_RDRF_Msk) {
        volatile uint32_t dummy = SPI0_REGS->SPI_RDR;
        (void) dummy;
    }

    dev->raw = rx;
    dev->state = ADS8329_STATE_DONE;

    /* Disable RX interrupt */
    SPI0_REGS->SPI_IDR = SPI_IDR_RDRF_Msk;

    __DSB(); // Dam bao moi thu hoan tat truoc khi nha CS

    /* CS High */
    PIOD_REGS->PIO_SODR = (uint32_t) 1U << PHOTO_ADC_CS_PIN;
}