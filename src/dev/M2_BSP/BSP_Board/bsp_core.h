#ifndef  __BSP_CORE_H__
#define __BSP_CORE_H__

#include "board.h"
#include "do.h"
#include "spi_io.h"
#include "i2c_io.h"
#include "M3_Driver/devices/MB85RS2MT/mb85rs2mt.h"
#include "M3_Driver/devices/IS66WVS8M8/is66wvs8m8.h"


//extern spi_io_t onboard_adc_spi;
extern spi_io_t spi0;
extern spi_io_t spi1;
extern do_t laser_dac_cs ;
extern do_t laser_dac_latch;
extern do_t laser_sw_int_cs ;
extern do_t laser_sw_ext_cs ;
 
extern do_t photo_sw_cs;
extern do_t photo_adc_cs;
extern do_t photo_adc_conv;
extern do_t photo_adc_eoc;

extern do_t power_som;
extern do_t power_buck_peri;
extern do_t power_tec;
extern do_t power_hd4;
extern do_t power_solenoid;
extern do_t power_lp;
extern do_t power_heater;

extern do_t status_led;

extern spi_io_t tec_usart_spi;
extern do_t tec_1_cs;
extern do_t tec_1_sw;
extern do_t tec_2_cs;
extern do_t tec_2_sw;
extern do_t tec_3_cs;
extern do_t tec_3_sw;
extern do_t tec_4_cs;
extern do_t tec_4_sw;

extern do_t bmp390_ext_sensor5;
extern i2c_io_t i2c0;
extern i2c_io_t i2c1;
extern i2c_io_t i2c2;

extern do_t sol1_in1;
extern do_t sol1_in2;
extern do_t sol2_in1;
extern do_t sol2_in2;
extern do_t sol3_in1;
extern do_t sol3_in2;
extern do_t sol4_in1;
extern do_t sol4_in2;

extern do_t fram_cs;
extern mb85rs2mt_dev_t g_fram;

extern do_t hd4_en;
extern do_t psram_cs;
extern is66_dev_t g_psram;

extern do_t flow1_en;
extern do_t flow2_en;
extern do_t flow3_en;
extern do_t flow4_en;

extern do_t lsm6_int1;

extern do_t wdt_done;
extern do_t wdt_wake;

extern do_t switch_spi_psram;

extern do_t mcu_pmu_gpio_a;
extern do_t mcu_pmu_gpio_b;

void BSP_board_init(void);

#endif /* __BSP_CORE_H__ */
