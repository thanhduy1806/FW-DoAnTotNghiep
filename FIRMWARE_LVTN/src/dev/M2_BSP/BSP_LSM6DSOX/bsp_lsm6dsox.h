#ifndef BSP_LSM6DSOX_H
#define	BSP_LSM6DSOX_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "M3_Driver/components/i2c/i2c_io.h"
#include "M3_Driver/devices/LSM6DSOX/lsm6dsox.h"
    
Std_ReturnType bsp_lsm6dsox_init();
Std_ReturnType bsp_lsm6dsox_disable_int();
Std_ReturnType bsp_lsm6dsox_check();

Std_ReturnType bsp_lsm6dsox_conf_int(uint8_t mode, uint8_t event);
Std_ReturnType bsp_lsm6dsox_conf_accel(uint8_t odr, uint8_t fs);
Std_ReturnType bsp_lsm6dsox_conf_gyro(uint8_t odr, uint8_t fs);
Std_ReturnType bsp_lsm6dsox_conf_ff(uint8_t ths, uint8_t dur);
Std_ReturnType bsp_lsm6dsox_conf_wu(uint8_t ths, uint8_t dur);

Std_ReturnType bsp_lsm6dsox_callback(void);

Std_ReturnType bsp_lsm6dsox_read_accel();
Std_ReturnType bsp_lsm6dsox_read_gyro();
Std_ReturnType bsp_lsm6dsox_read_temp();
Std_ReturnType bsp_lsm6dsox_read_all();

#ifdef	__cplusplus
}
#endif

#endif	/* BSP_LSM6DSOX_H */
