#include "lsm6dsox.h"
/* ====================================================== Private defines ====================================================== */

/* Sensitivity (datasheet Table 2) */
#define SENS_XL_2G      (0.061f)    /* mg/LSB  */
#define SENS_XL_4G      (0.122f)
#define SENS_XL_8G      (0.244f)
#define SENS_XL_16G     (0.488f)

#define SENS_GY_125     (0.004375f)    /* dps/LSB */
#define SENS_GY_250     (0.008750f)
#define SENS_GY_500     (0.017500f)
#define SENS_GY_1000    (0.035000f)
#define SENS_GY_2000    (0.070000f)

#define TEMP_SENS       (256.0f)    /* LSB/�C  */
#define TEMP_OFFSET     (25.0f)     /* �C      */

/* ====================================================== Variables ====================================================== */
static lsm6d_rawData_t lsm6d_rawData;
static uint16_t xl_odr;
static uint8_t xl_fs;
static uint16_t gy_odr;
static uint8_t gy_fs;

static const lsm6d_xl_fs_t xl_fs_table[] = {
    LSM6D_XL_FS_2G,
    LSM6D_XL_FS_16G,
    LSM6D_XL_FS_4G,
    LSM6D_XL_FS_8G
};

static const lsm6d_xl_odr_t xl_odr_table[] = {
    LSM6D_XL_ODR_OFF,
    LSM6D_XL_ODR_12Hz5,     /*1*/
    LSM6D_XL_ODR_26Hz,
    LSM6D_XL_ODR_52Hz,
    LSM6D_XL_ODR_104Hz,     /*4*/
    LSM6D_XL_ODR_208Hz,
    LSM6D_XL_ODR_416Hz,
    LSM6D_XL_ODR_833Hz      /*7*/
};

static const lsm6d_gy_fs_t gy_fs_table[] = {
    LSM6D_GY_FS_125dps,
    LSM6D_GY_FS_250dps,
    LSM6D_GY_FS_500dps,     /*2*/
    LSM6D_GY_FS_1000dps,
    LSM6D_GY_FS_2000dps     /*4*/
};

static const lsm6d_gy_odr_t gy_odr_table[] = {
    LSM6D_GY_ODR_OFF,       /*0*/
    LSM6D_GY_ODR_12Hz5,
    LSM6D_GY_ODR_26Hz,
    LSM6D_GY_ODR_52Hz,
    LSM6D_GY_ODR_104Hz,     /*4*/
    LSM6D_GY_ODR_208Hz,
    LSM6D_GY_ODR_416Hz,
    LSM6D_GY_ODR_833Hz      /*7*/
};

/* ====================================================== Private functions ====================================================== */
static uint8_t lsm6d_write(lsm6d_dev_t* dev, uint8_t reg, uint8_t TX_data) {
    uint8_t frame[2] = {reg, TX_data};

    uint32_t n = i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char*) frame, 2);
}

static uint8_t lsm6d_read(lsm6d_dev_t* dev, uint8_t reg, uint8_t *p_RX_buffer, uint8_t byte_count) {
    // 1) Write the register pointer WITHOUT STOP
    if (i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char*) &reg, 1) != 0u) {
        return ERROR_FAIL;
    }

    // 2) Next recv performs a REPEATED-START automatically, then STOP
    uint32_t n = i2c_io_recv(dev->i2c_bus, dev->slaveAdd, (char*) p_RX_buffer, (int) byte_count);
}

static Std_ReturnType _modify_reg(lsm6d_dev_t* dev, uint8_t reg, uint8_t mask, uint8_t val)
{
    uint8_t tmp = 0;
    Std_ReturnType r = lsm6d_read(dev, reg, &tmp, 1);
    if (r != ERROR_OK) return r;
    tmp = (tmp & ~mask) | (val & mask);
    return lsm6d_write(dev, reg, tmp);
}

static float _xl_sensitivity(lsm6d_xl_fs_t fs)
{
    switch (fs)
    {
        case LSM6D_XL_FS_2G:  return SENS_XL_2G;
        case LSM6D_XL_FS_4G:  return SENS_XL_4G;
        case LSM6D_XL_FS_8G:  return SENS_XL_8G;
        case LSM6D_XL_FS_16G: return SENS_XL_16G;
        default:              return SENS_XL_2G;
    }
}

static float _gy_sensitivity(lsm6d_gy_fs_t fs)
{
    switch (fs)
    {
        case LSM6D_GY_FS_125dps:  return SENS_GY_125;
        case LSM6D_GY_FS_250dps:  return SENS_GY_250;
        case LSM6D_GY_FS_500dps:  return SENS_GY_500;
        case LSM6D_GY_FS_1000dps: return SENS_GY_1000;
        case LSM6D_GY_FS_2000dps: return SENS_GY_2000;
        default:                  return SENS_GY_250;
    }
}

/* ====================================================== Configuration ====================================================== */
Std_ReturnType lsm6d_set_accel_odr(lsm6d_dev_t* dev, uint8_t odr)
{
    xl_odr = xl_odr_table[odr];
}

Std_ReturnType lsm6d_set_accel_fs(lsm6d_dev_t* dev, uint8_t fs){
    xl_fs = xl_fs_table[fs];
    dev->xl_fs = xl_fs;
}

Std_ReturnType lsm6d_set_gyro_odr(lsm6d_dev_t* dev, uint8_t odr)
{
    gy_odr = gy_odr_table[odr];
}

Std_ReturnType lsm6d_set_gyro_fs(lsm6d_dev_t* dev, uint8_t fs){
    gy_fs = gy_fs_table[fs];
    dev->gy_fs = gy_fs;
}


Std_ReturnType lsm6d_init(lsm6d_dev_t *dev)
{
    if(dev == NULL) return ERROR_INVALID_PARAM;
    
    dev->init_status = false;

    uint8_t id = 0;
    lsm6d_read_id(dev, &id);
    if (id != LSM6D_ID) return ERROR_FAIL;
    
    lsm6d_write(dev, LSM6D_CTRL1_XL, xl_odr | xl_fs);
    lsm6d_write(dev, LSM6D_CTRL2_G, gy_odr | gy_fs);
    
    dev->init_status = true;
    return ERROR_OK;
}

/* ================================================ Interrupt Configuration ================================================ */
Std_ReturnType lsm6d_set_int1_ctrl(lsm6d_dev_t* dev, lsm6d_int_ctrl_t* cfg)
{
    uint8_t reg_val = 0;

    if (cfg->drdy_xl)   reg_val |= (1 << 0);
    if (cfg->drdy_g)    reg_val |= (1 << 1);
    if (cfg->drdy_temp) reg_val |= (1 << 2);
    if (cfg->fifo_th)   reg_val |= (1 << 3);
    if (cfg->fifo_ovr)  reg_val |= (1 << 4);
    if (cfg->fifo_full) reg_val |= (1 << 5);
    if (cfg->cnt_bdr)   reg_val |= (1 << 6);
    
    return lsm6d_write(dev, LSM6D_INT1_CTRL, reg_val);
}

Std_ReturnType lsm6d_set_int1_event(lsm6d_dev_t* dev, lsm6d_int_event_t* cfg)
{
    _modify_reg(dev, LSM6D_TAP_CFG2, 0x80, 0x80);
    
    uint8_t reg_val = 0;
    if (cfg->int_shub)       reg_val |= (1 << 0);
    if (cfg->int_emb_func)   reg_val |= (1 << 1);
    if (cfg->int_6d)         reg_val |= (1 << 2);
    if (cfg->int_double_tap) reg_val |= (1 << 3);
    if (cfg->int_ff)         reg_val |= (1 << 4);
    if (cfg->int_wu)         reg_val |= (1 << 5);
    if (cfg->int_single_tap) reg_val |= (1 << 6);
    if (cfg->int_sleep_chg)  reg_val |= (1 << 7);
    
    Std_ReturnType r;
    r = lsm6d_write(dev, LSM6D_MD1_CFG, reg_val);
    if(r != ERROR_OK) return r;

    uint8_t dummy;
    lsm6d_read(dev, LSM6D_ALL_INT_SRC, &dummy, 1);
}

Std_ReturnType lsm6d_set_wakeup(lsm6d_dev_t* dev, lsm6d_wakeup_cfg_t* cfg)
{
    if (dev == NULL || cfg == NULL) return ERROR_INVALID_PARAM;

    Std_ReturnType r;

    /* Enable slope filter + apply threshold */
     r = _modify_reg(dev, LSM6D_TAP_CFG0, 0x10, 0x10);  /* SLOPE_FDS = 1 */
     if (r != ERROR_OK) return r;

    /* WAKE_UP_THS bits [5:0] = threshold */
    r = _modify_reg(dev, LSM6D_WAKE_UP_THS, 0x3F, cfg->threshold & 0x3F);
    if (r != ERROR_OK) return r;

    /* WAKE_UP_DUR bits [5:4] = duration */
    r = _modify_reg(dev, LSM6D_WAKE_UP_DUR, 0x60,
                    (uint8_t)((cfg->duration & 0x03) << 4));
    
    return r;
}

Std_ReturnType lsm6d_set_freefall(lsm6d_dev_t* dev, lsm6d_ff_cfg_t* cfg)
{
    if (dev == NULL || cfg == NULL) return ERROR_INVALID_PARAM;

    Std_ReturnType r;

    /* FREE_FALL register:
     * bits [2:0] = FF_THS (threshold)
     * bits [7:3] = FF_DUR (duration) ? lower 5 bits
     * WAKE_UP_DUR bit [7] = FF_DUR[5] ? upper 1 bit */

    uint8_t val = (uint8_t)(((cfg->duration & 0x1F) << 3) | (cfg->threshold & 0x07));
    r = lsm6d_write(dev, LSM6D_FREE_FALL, val);
    if (r != ERROR_OK) return r;

    /* FF_DUR MSb ? WAKE_UP_DUR bit[7] */
    r = _modify_reg(dev, LSM6D_WAKE_UP_DUR, 0x80,
                    (uint8_t)(((cfg->duration >> 5) & 0x01) << 7));
    return r;
}

/* ================================================ Read individual ================================================ */
Std_ReturnType lsm6d_get_status(lsm6d_dev_t* dev, uint8_t* status)
{
    if (dev == NULL || status == NULL) return ERROR_INVALID_PARAM;
    return lsm6d_read(dev, LSM6D_STATUS_REG, status, 1);
}

bool lsm6d_is_accel_ready(lsm6d_dev_t* dev)
{
    uint8_t status = 0;
    lsm6d_get_status(dev, &status);
    return (status & LSM6D_XLDA) != 0;
}

bool lsm6d_is_gyro_ready(lsm6d_dev_t* dev)
{
    uint8_t status = 0;
    lsm6d_get_status(dev, &status);
    return (status & LSM6D_GDA) != 0;
}

Std_ReturnType lsm6d_read_accel(lsm6d_dev_t* dev, Accel_Gyro_t* accel)
{
    if (dev == NULL || accel == NULL) return ERROR_INVALID_PARAM;
    if (!dev->init_status) return ERROR_NOT_READY;
    
    uint8_t buf[6];
    Std_ReturnType r = lsm6d_read(dev, LSM6D_OUTX_L_A, buf, 6);
    if (r != ERROR_OK) return r;

    float sens = _xl_sensitivity(dev->xl_fs);
    accel->x = (float)(int16_t)((buf[1] << 8) | buf[0]) * sens;
    accel->y = (float)(int16_t)((buf[3] << 8) | buf[2]) * sens;
    accel->z = (float)(int16_t)((buf[5] << 8) | buf[4]) * sens;

    return ERROR_OK;
}

Std_ReturnType lsm6d_read_gyro(lsm6d_dev_t* dev, Accel_Gyro_t* gyro)
{
    if (dev == NULL || gyro == NULL) return ERROR_INVALID_PARAM;
    if (!dev->init_status) return ERROR_NOT_READY;
    
    uint8_t buf[6];
    Std_ReturnType r = lsm6d_read(dev, LSM6D_OUTX_L_G, buf, 6);
    if (r != ERROR_OK) return r;
    
    float sens = _gy_sensitivity(dev->gy_fs);
    gyro->x = (float)(int16_t)((buf[1] << 8) | buf[0]) * sens;
    gyro->y = (float)(int16_t)((buf[3] << 8) | buf[2]) * sens;
    gyro->z = (float)(int16_t)((buf[5] << 8) | buf[4]) * sens;

    return ERROR_OK;
}

Std_ReturnType lsm6d_read_temp(lsm6d_dev_t* dev, float* temp)
{
    if (dev == NULL || temp == NULL) return ERROR_INVALID_PARAM;
    if (!dev->init_status) return ERROR_NOT_READY;

    uint8_t buf[2];
    Std_ReturnType r = lsm6d_read(dev, LSM6D_OUT_TEMP_L, buf, 2);
    if (r != ERROR_OK) return r;

    int16_t raw = (int16_t)((buf[1] << 8) | buf[0]);
    *temp = ((float)raw / TEMP_SENS) + TEMP_OFFSET;

    return ERROR_OK;
}

Std_ReturnType lsm6d_read_all(lsm6d_dev_t* dev, lsm6d_data_t* data)
{
    if (dev == NULL || data == NULL) return ERROR_INVALID_PARAM;
    if (!dev->init_status) return ERROR_NOT_READY;

    uint8_t buf[14];
    Std_ReturnType r = lsm6d_read(dev, LSM6D_OUT_TEMP_L, buf, 14);
    if (r != ERROR_OK) return r;

    /* Temperature */
    int16_t raw = (int16_t)((buf[1] << 8) | buf[0]);
    data->Temperature = ((float)raw / TEMP_SENS) + TEMP_OFFSET;

    /* Gyroscope */
    float g_sens = _gy_sensitivity(dev->gy_fs);
    data->Gyro.x = (float)(int16_t)((buf[3]  << 8) | buf[2])  * g_sens;
    data->Gyro.y = (float)(int16_t)((buf[5]  << 8) | buf[4])  * g_sens;
    data->Gyro.z = (float)(int16_t)((buf[7]  << 8) | buf[6])  * g_sens;

    /* Accelerometer */
    float xl_sens = _xl_sensitivity(dev->xl_fs);
    data->Accel.x = (float)(int16_t)((buf[9]  << 8) | buf[8])  * xl_sens;
    data->Accel.y = (float)(int16_t)((buf[11] << 8) | buf[10]) * xl_sens;
    data->Accel.z = (float)(int16_t)((buf[13] << 8) | buf[12]) * xl_sens;

    return ERROR_OK;
}

Std_ReturnType lsm6d_read_id(lsm6d_dev_t *dev, uint8_t *id) {
    if (lsm6d_read(dev, LSM6D_ID_ADDR, id, 1))
        return ERROR_OK;
    return ERROR_FAIL;
}