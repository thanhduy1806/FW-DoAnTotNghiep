#ifndef BSP_SOLENOID_H
#define	BSP_SOLENOID_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "peripheral/pio/plib_pio.h"
#include "M3_Driver/components/i2c/i2c_io.h"
    
void bsp_solenoid_init();

uint32_t bsp_sol_single_on(uint8_t index);
uint32_t bsp_sol_single_off(uint8_t index);
uint32_t bsp_sol_single_status(uint8_t index);

#ifdef	__cplusplus
}
#endif

#endif	/* BSP_SOLENOID_H */
