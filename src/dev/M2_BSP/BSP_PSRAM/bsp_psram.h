/* 
 * File:   bsp_psram.h
 * Author: HTSANG
 *
 * Created on April 3, 2026, 3:01 PM
 */

#ifndef BSP_PSRAM_H
#define	BSP_PSRAM_H

#include "M3_Driver/devices/IS66WVS8M8/is66wvs8m8.h"

void bsp_psram_sw_mcu(void);

void bsp_psram_sw_mpu(void);

int bsp_psram_read_id(is66_dev_t *dev);

int bsp_psram_write(is66_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);

int bsp_psram_read(is66_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);

#endif	/* BSP_PSRAM_H */

