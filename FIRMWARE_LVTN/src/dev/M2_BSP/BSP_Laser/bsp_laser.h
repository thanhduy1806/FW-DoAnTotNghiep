#ifndef BSP_BSP_LASER_H_
#define BSP_BSP_LASER_H_

#include "stdint.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void bsp_laser_init(void);

void bsp_laser_int_set_dac(uint8_t code);
uint8_t bsp_laser_int_get_dac(void);

void bsp_laser_ext_set_dac(uint8_t code);
uint8_t bsp_laser_ext_get_dac(void);

void bsp_laser_int_sw_on(uint8_t channel);
void bsp_laser_int_sw_off(uint8_t channel);
//void bsp_laser_int_sw_on_manual(uint8_t channel);
//void bsp_laser_int_sw_off_manual(uint8_t channel);
void bsp_laser_int_all_sw_off(void);

void bsp_laser_ext_sw_on(uint8_t channel);
void bsp_laser_ext_sw_off(uint8_t channel);
void bsp_laser_ext_sw_on_manual(uint8_t channel);
void bsp_laser_ext_sw_off_manual(uint8_t channel);
void bsp_laser_ext_all_sw_off(void);

void bsp_laser_setup_timmer(uint32_t sampling_rate_khz);
void bsp_laser_start_timer(void);

/* ============================================================
 * 1) TRIGGER: select channel and start a one-shot conversion
 *    - Channel 0 -> ADC1_IN0
 *    - Channel 1 -> ADC1_IN1
 * ============================================================ */
void bsp_laser_int_current_trigger_adc(void);
void bsp_laser_ext_current_trigger_adc(void);

/* ============================================================
 * 2) READ DATA (assumes conversion already finished)
 *    Reads PCDR[x] that corresponds to the selected channel.
 * ============================================================ */
uint16_t bsp_laser_int_current_read_adc_data(void);
uint16_t bsp_laser_ext_current_read_adc_data(void);

/* ============================================================
 * 3) POLLING READ (trigger -> wait -> read)
 *    Busy-waits on EOC/ECH with no timeout per your request.
 *    If you want a timeout later, add a countdown in the loop.
 * ============================================================ */
uint16_t bsp_laser_int_current_adc_polling(void);
uint16_t bsp_laser_ext_current_adc_polling(void);

#endif /* BSP_BSP_LASER_H_ */
