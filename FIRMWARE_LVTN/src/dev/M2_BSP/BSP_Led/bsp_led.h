/* 
 * File:   bsp_led.h
 * Author: HTSANG
 *
 * Created on February 27, 2026, 11:39 AM
 */

#ifndef BSP_LED_H
#define	BSP_LED_H

#ifdef	__cplusplus
extern "C" {
#endif


void bsp_led_set(void);

void bsp_led_reset(void);

void bsp_led_toggle(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BSP_LED_H */

