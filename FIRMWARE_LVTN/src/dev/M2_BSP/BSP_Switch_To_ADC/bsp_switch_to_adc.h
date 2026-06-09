#ifndef BSP_SWITCH_TO_ADC_H_
#define BSP_SWITCH_TO_ADC_H_

#include <stdint.h>
#include <stdio.h>
#include "peripheral/pio/plib_pio.h"
#include "M3_Driver/components/i2c/i2c_io.h"
#include "M3_Driver/components/adc/adc.h"

#define ADC_POWER_CHANNEL                                   AFEC_CH2
#define CALIBORATION_VALUE                                  10                                       

// Function prototypes for switch to ADC operations
void bsp_switch_to_adc_init(void);
uint8_t bsp_switch_to_adc_on(uint8_t switch_number);
uint8_t bsp_switch_to_adc_off(uint8_t switch_number);
uint8_t bsp_switch_to_adc_off_all(void);
uint8_t bsp_switch_to_adc_status(uint8_t switch_number);
uint16_t bsp_all_switch_to_adc_status(void);
uint16_t bsp_get_adc_value_monitor(uint8_t switch_number);
float bsp_get_voltage_value(uint8_t switch_number);
float bsp_get_current_value_power(uint8_t switch_number);


#endif /* BSP_SWITCH_TO_ADC_H_ */