/* 
 * File:   bsp_photo.h
 * Author: HTSANG
 *
 * Created on March 18, 2026, 1:19 PM
 */

#ifndef BSP_PHOTO_H
#define	BSP_PHOTO_H

#include "stdint.h"
#include "os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "M3_Driver/devices/ADG1414BR/adg1414.h"
#include "M3_Driver/devices/ADS8329/ads8329.h"

#define PHOTO_CHANNEL_MAX 4

extern adg1414_dev_t photo_sw_dev;
extern ads8329_dev_t photo_adc_dev;

void bsp_photo_init(void);

void bsp_photo_sw_on(uint8_t channel);
void bsp_photo_sw_off(uint8_t channel);
void bsp_photo_all_sw_off(void);

void bsp_photo_start_sampling(void);
void bsp_photo_stop_sampling(void);



#endif	/* BSP_PHOTO_H */

