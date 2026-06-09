/* 
 * File:   bsp_heater.h
 * Author: HTSANG
 *
 * Created on February 27, 2026, 5:52 PM
 */

#ifndef BSP_HEATER_H
#define	BSP_HEATER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef enum {
    HTR_1 = 0, HTR_2, HTR_3, HTR_4, HTR_5, HTR_6, HTR_7, HTR_8,
} heater_channel_t;

void bsp_heater_set_duty(heater_channel_t HTR_CH, uint8_t HTR_DUTY);
void bsp_heater_off(heater_channel_t HTR_CH);



#ifdef	__cplusplus
}
#endif

#endif	/* BSP_HEATER_H */

