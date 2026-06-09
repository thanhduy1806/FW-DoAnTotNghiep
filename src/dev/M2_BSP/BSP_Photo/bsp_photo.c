#include "bsp_photo.h"
#include "bsp_core.h"
#include "adg1414.h"
#include "peripheral/tc/plib_tc0.h"

#include "M0_App/AppOS/App_Experiment/app_experiment.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//static uint16_t photo_data[24][1] = {0};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int8_t map_PD_position(int x);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//uint32_t volatile photo_spi_count	 = 0;
//uint32_t volatile current_channel	 = 0;
//uint32_t volatile  photo_spi_set_count = 0;
//uint8_t volatile  is_spi_counter_finish = 0;

adg1414_dev_t photo_sw_dev = {
    .spi = &spi0,
    .cs = &photo_sw_cs,
    .num_of_sw = 3
};

ads8329_dev_t photo_adc_dev = {
    .spi = &spi0,
    .cs = &photo_adc_cs,
    .conv = &photo_adc_conv,
    .eoc = &photo_adc_eoc,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void bsp_photo_init(void) {
    spi_io_set_frame_size(photo_sw_dev.spi, 8);
    spi_io_set_prescale(photo_sw_dev.spi, SPI0_COMMON_PRESCALE);
    adg1414_chain_init(&photo_sw_dev, photo_sw_dev.spi, photo_sw_dev.cs, photo_sw_dev.num_of_sw);
    ads8329_init(&photo_adc_dev);
    spi_io_set_prescale(photo_sw_dev.spi, PHOTO_ADC_PRESCALE);
    spi_io_set_frame_size(photo_sw_dev.spi, 16);
}

void bsp_photo_sw_on(uint8_t channel) {
    spi_io_set_frame_size(photo_sw_dev.spi, 8);
    spi_io_set_prescale(photo_sw_dev.spi, SPI0_COMMON_PRESCALE);
    uint8_t real_channel = map_PD_position(channel);
    adg1414_chain_sw_on(&photo_sw_dev, real_channel);
    spi_io_set_prescale(photo_sw_dev.spi, PHOTO_ADC_PRESCALE);
    spi_io_set_frame_size(photo_sw_dev.spi, 16);
}

void bsp_photo_sw_off(uint8_t channel) {
    spi_io_set_frame_size(photo_sw_dev.spi, 8);
    spi_io_set_prescale(photo_sw_dev.spi, SPI0_COMMON_PRESCALE);
    uint8_t real_channel = map_PD_position(channel);
    adg1414_chain_sw_off(&photo_sw_dev, real_channel);
    spi_io_set_prescale(photo_sw_dev.spi, PHOTO_ADC_PRESCALE);
    spi_io_set_frame_size(photo_sw_dev.spi, 16);
}

void bsp_photo_all_sw_off(void) {
    spi_io_set_frame_size(photo_sw_dev.spi, 8);
    spi_io_set_prescale(photo_sw_dev.spi, SPI0_COMMON_PRESCALE);
    adg1414_chain_all_sw_off(&photo_sw_dev);
    spi_io_set_prescale(photo_sw_dev.spi, PHOTO_ADC_PRESCALE);
    spi_io_set_frame_size(photo_sw_dev.spi, 16);
}

void bsp_photo_start_sampling(void) {
    TC0_CH1_CompareStart();
    return;
}

void bsp_photo_stop_sampling(void) {
    TC0_CH1_CompareStop();
    return;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int8_t map_PD_position(int x) {
    static const uint8_t map[] = {
        0,
        24, 23, 22, 21, 17, 18, 19, 20,
        16, 15, 14, 13, 9, 10, 11, 12,
        8, 7, 6, 5, 1, 2, 3, 4
    };

    if (x < 1 || x > 24) {
        return -1;
    }

    return map[x];
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */