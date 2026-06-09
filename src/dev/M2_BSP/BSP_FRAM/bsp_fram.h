/* 
 * File:   bsp_fram.h
 * Author: HTSANG
 *
 * Created on March 27, 2026, 1:47 PM
 */

#ifndef BSP_FRAM_H
#define	BSP_FRAM_H

#include "M3_Driver/devices/MB85RS2MT/mb85rs2mt.h"

int bsp_fram_write(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);

int bsp_fram_read(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);

#endif	/* BSP_FRAM_H */