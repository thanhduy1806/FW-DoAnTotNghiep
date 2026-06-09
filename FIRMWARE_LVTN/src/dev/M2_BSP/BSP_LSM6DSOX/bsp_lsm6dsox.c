#include "bsp_lsm6dsox.h"
#include "bsp_core.h"

#include "../BSP_Led/bsp_led.h"

#include "../../M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"

extern i2c_io_t i2c1;
lsm6d_data_t    lsm6d_all_data = {0};
Accel_Gyro_t    lsm6d_accel_data = {0};
Accel_Gyro_t    lsm6d_gyro_data = {0};
float           lsm6d_temperature = 0.0f;

lsm6d_dev_t lsm6dsox = {
    .i2c_bus = &i2c1,
    .slaveAdd = LSM6D_ADDRESS_1,
};

Std_ReturnType bsp_lsm6dsox_conf_accel(uint8_t odr, uint8_t fs)
{
    lsm6d_set_accel_odr(&lsm6dsox, odr);
    lsm6d_set_accel_fs(&lsm6dsox, fs);
    
    return ERROR_OK;
}

Std_ReturnType bsp_lsm6dsox_conf_gyro(uint8_t odr, uint8_t fs)
{
    lsm6d_set_gyro_odr(&lsm6dsox, odr);
    lsm6d_set_gyro_fs(&lsm6dsox, fs);
    
    return ERROR_OK;
}

Std_ReturnType bsp_lsm6dsox_init()
{
    return lsm6d_init(&lsm6dsox);
}

Std_ReturnType bsp_lsm6dsox_conf_int(uint8_t mode, uint8_t event)
{
    lsm6d_int_ctrl_t int1_config = {0};
    lsm6d_int_event_t int1_event = {0};
    
    switch (mode) {
        case 0: memset(&int1_config, 0, sizeof(int1_config)); break;
        case 1: int1_config.drdy_xl   = true;   break;
        case 2: int1_config.drdy_g    = true;   break;
        case 3: int1_config.drdy_temp = true;   break;
        case 4: int1_config.fifo_th   = true;   break;
        case 5: int1_config.fifo_ovr  = true;   break;
        case 6: int1_config.fifo_full = true;   break;
        case 7: int1_config.cnt_bdr   = true;   break;
        default: break;
    }
    
    switch (event) {
        case 0: memset(&int1_event, 0, sizeof(int1_event));   break;
        case 1: int1_event.int_shub       = true;   break;
        case 2: int1_event.int_emb_func   = true;   break;
        case 3: int1_event.int_6d         = true;   break;
        case 4: int1_event.int_double_tap = true;   break;
        case 5: int1_event.int_ff         = true;   break;
        case 6: int1_event.int_wu         = true;   break;
        case 7: int1_event.int_single_tap = true;   break;
        case 8: int1_event.int_sleep_chg  = true;   break;
        default: break;
    }
    
    Std_ReturnType r = ERROR_OK;
    r |= lsm6d_set_int1_event(&lsm6dsox, &int1_event);
    r |= lsm6d_set_int1_ctrl(&lsm6dsox, &int1_config);
    return r;
}

Std_ReturnType bsp_lsm6dsox_conf_ff(uint8_t ths, uint8_t dur)
{
    lsm6d_ff_cfg_t ff_cfg = {0};
    
    ff_cfg.threshold = ths;
    ff_cfg.duration = dur;
    
    return lsm6d_set_freefall(&lsm6dsox, &ff_cfg);
}

Std_ReturnType bsp_lsm6dsox_conf_wu(uint8_t ths, uint8_t dur)
{
    lsm6d_wakeup_cfg_t wu_cfg = {0};
    
    wu_cfg.threshold = ths;
    wu_cfg.duration = dur;
    
    return lsm6d_set_wakeup(&lsm6dsox, &wu_cfg);
}

Std_ReturnType bsp_lsm6dsox_disable_int()
{
    Std_ReturnType status = ERROR_OK;
    
    lsm6d_int_ctrl_t int1_config = {0};
    lsm6d_int_event_t int1_event = {0};
    status |= lsm6d_set_int1_ctrl(&lsm6dsox, &int1_config);
    status |= lsm6d_set_int1_event(&lsm6dsox, &int1_event);
    
    return (status == ERROR_OK) ? ERROR_OK : status;
}

Std_ReturnType bsp_lsm6dsox_check()
{
    uint8_t ID = 0;
	lsm6d_read_id(&lsm6dsox, &ID);
	if (ID != LSM6D_ID) return ERROR_I2C_BUS;
    return ERROR_OK;
}


Std_ReturnType bsp_lsm6dsox_read_accel()
{
    return lsm6d_read_accel(&lsm6dsox, &lsm6d_accel_data);
}

Std_ReturnType bsp_lsm6dsox_read_gyro()
{
    return lsm6d_read_gyro(&lsm6dsox, &lsm6d_gyro_data);
}

Std_ReturnType bsp_lsm6dsox_read_temp()
{
    return lsm6d_read_temp(&lsm6dsox, &lsm6d_temperature);
}

Std_ReturnType bsp_lsm6dsox_read_all()
{
    return lsm6d_read_all(&lsm6dsox, &lsm6d_all_data);
}

Std_ReturnType bsp_lsm6dsox_callback(void)
{
/* NOTE: Interrupt handling logic is NOT defined yet.
     * Implementation should include:
     * 1. Reading the interrupt source register (e.g., ALL_INT_SRC).
     * 2. Handling specific events (Wake-up, Free-fall, or Data Ready).
     * 3. Clearing flags if necessary.
     */
     
    return ERROR_OK;
}