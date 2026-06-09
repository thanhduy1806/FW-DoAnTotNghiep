#ifndef LSM6DSOX_H
#define	LSM6DSOX_H

#include "stdint.h"
#include "stdbool.h"
#include "i2c_io.h"
#include "define.h"
#include "M2_BSP/BSP_Board/bsp_core.h"

/* ============================================================ Defines ============================================================ */

#define LSM6D_ID		         0x6C
#define LSM6D_ADDRESS_0          0x6A
#define LSM6D_ADDRESS_1          0x6B

/* --- Registers --- */
#define LSM6D_ID_ADDR            0x0F
#define LSM6D_CTRL1_XL           0X10
#define LSM6D_CTRL2_G            0X11
#define LSM6D_CTRL3_C            0X12
#define LSM6D_CTRL4_C            0x13
#define LSM6D_CTRL5_C            0x14
#define LSM6D_CTRL6_C            0x15
#define LSM6D_CTRL7_G            0x16
#define LSM6D_CTRL8_XL           0x17
#define LSM6D_CTRL9_XL           0x18
#define LSM6D_CTRL10_C           0x19
#define LSM6D_INT1_CTRL          0x0D
#define LSM6D_INT2_CTRL          0x0E
#define LSM6D_STATUS_REG         0X1E
#define LSM6D_OUT_TEMP_L         0x20
#define LSM6D_OUTX_L_G           0x22
#define LSM6D_OUTX_L_A           0x28
#define LSM6D_TAP_CFG0           0x56
#define LSM6D_TAP_CFG2           0x58
#define LSM6D_MD1_CFG            0x5E  /* INT1 function routing */
#define LSM6D_MD2_CFG            0x5F  /* INT2 function routing */
#define LSM6D_WAKE_UP_SRC        0x1B
#define LSM6D_WAKE_UP_THS        0x5B
#define LSM6D_WAKE_UP_DUR        0x5C
#define LSM6D_FREE_FALL          0x5D
#define LSM6D_ALL_INT_SRC        0x1A

/* --- CTRL3_C bits --- */
#define LSM6D_SW_RESET           0x01
#define LSM6D_IF_INC             0x04
#define LSM6D_BDU                0x40

/* --- STATUS bits --- */
#define LSM6D_XLDA               0x01  /* Accel data ready  */
#define LSM6D_GDA                0x02  /* Gyro data ready   */
#define LSM6D_TDA                0x04  /* Temp data ready   */

#define GYRO_SENSITIVITY_500DPS 	0.01750f 	// dps/LSB
#define ACCEL_SENSITIVITY_8G		0.244f		// mg / LSB

/* ============================================================ Enums ============================================================ */

/**
 * @brief Accelerometer Output Data Rate
 */
typedef enum {
    LSM6D_XL_ODR_OFF    = 0x00,
    LSM6D_XL_ODR_12Hz5  = 0x10,
    LSM6D_XL_ODR_26Hz   = 0x20,
    LSM6D_XL_ODR_52Hz   = 0x30,
    LSM6D_XL_ODR_104Hz  = 0x40,
    LSM6D_XL_ODR_208Hz  = 0x50,
    LSM6D_XL_ODR_416Hz  = 0x60,
    LSM6D_XL_ODR_833Hz  = 0x70,
} lsm6d_xl_odr_t;

/**
 * @brief Accelerometer Full-Scale
 */
typedef enum {
    LSM6D_XL_FS_2G  = 0x00,
    LSM6D_XL_FS_16G = 0x04,
    LSM6D_XL_FS_4G  = 0x08,
    LSM6D_XL_FS_8G  = 0x0C,
} lsm6d_xl_fs_t;

/**
 * @brief Gyroscope Output Data Rate
 */
typedef enum {
    LSM6D_GY_ODR_OFF    = 0x00,
    LSM6D_GY_ODR_12Hz5  = 0x10,
    LSM6D_GY_ODR_26Hz   = 0x20,
    LSM6D_GY_ODR_52Hz   = 0x30,
    LSM6D_GY_ODR_104Hz  = 0x40,
    LSM6D_GY_ODR_208Hz  = 0x50,
    LSM6D_GY_ODR_416Hz  = 0x60,
    LSM6D_GY_ODR_833Hz  = 0x70,
} lsm6d_gy_odr_t;

/**
 * @brief Gyroscope Full-Scale
 */
typedef enum {
    LSM6D_GY_FS_125dps  = 0x02,
    LSM6D_GY_FS_250dps  = 0x00,
    LSM6D_GY_FS_500dps  = 0x04,
    LSM6D_GY_FS_1000dps = 0x08,
    LSM6D_GY_FS_2000dps = 0x0C,
} lsm6d_gy_fs_t;

/**
 * @brief INT1/INT2 basic signals (INT1_CTRL / INT2_CTRL register)
 */
typedef struct {
    bool drdy_xl;       /* Accel data ready     */
    bool drdy_g;        /* Gyro data ready      */
    bool drdy_temp;     /* Temp data ready      */
    bool fifo_th;       /* FIFO threshold       */
    bool fifo_ovr;      /* FIFO overrun         */
    bool fifo_full;     /* FIFO full            */
    bool cnt_bdr;       /* Counter BDR          */
} lsm6d_int_ctrl_t;

/**
 * @brief INT1/INT2 event routing (MD1_CFG / MD2_CFG register)
 */
typedef struct {
    bool int_shub;      /* Sensor hub           */
    bool int_emb_func;  /* Embedded functions   */
    bool int_6d;        /* 6D orientation       */
    bool int_double_tap;/* Double tap           */
    bool int_ff;        /* Free-fall            */
    bool int_wu;        /* Wake-up              */
    bool int_single_tap;/* Single tap           */
    bool int_sleep_chg; /* Sleep change         */
} lsm6d_int_event_t;

/**
 * @brief Wake-up configuration
 */
typedef struct {
    uint8_t threshold;  /* Wake-up threshold (6-bit, 1 LSB = FS_XL/64) */
    uint8_t duration;   /* Wake-up duration  (2-bit, 1 LSB = 1/ODR_XL) */
} lsm6d_wakeup_cfg_t;

/**
 * @brief Free-fall configuration
 */
typedef struct {
    uint8_t threshold;  /* Free-fall threshold (3-bit, 0-7)             */
    uint8_t duration;   /* Free-fall duration  (6-bit, 1 LSB = 1/ODR)  */
} lsm6d_ff_cfg_t;

/* ============================================================ Types ============================================================ */

typedef struct lsm6d_dev {
    i2c_io_t* i2c_bus;
    uint8_t slaveAdd;
    lsm6d_xl_fs_t xl_fs;    /* L?u FS ?? tính sensitivity */
    lsm6d_gy_fs_t gy_fs;
    bool init_status;
} lsm6d_dev_t;

typedef struct lsm6d_rawData {
    uint8_t RxData[12];
} lsm6d_rawData_t;

typedef struct Accel_Gyro {
    float x;
    float y;
    float z;
} Accel_Gyro_t;

typedef struct lsm6d_data {
    Accel_Gyro_t Accel;         /* mg  */
    Accel_Gyro_t Gyro;          /* dps */
    float        Temperature;   /* °C  */
} lsm6d_data_t;

/* ============================================================ Prototypes ============================================================ */

/* --- Init --- */
Std_ReturnType lsm6d_init(lsm6d_dev_t *dev);
Std_ReturnType lsm6d_read_id(lsm6d_dev_t *dev, uint8_t *ID);

/* --- Configuration --- */
Std_ReturnType lsm6d_set_accel_odr(lsm6d_dev_t* dev, lsm6d_xl_odr_t odr);
Std_ReturnType lsm6d_set_accel_fs(lsm6d_dev_t* dev, lsm6d_xl_fs_t fs);
Std_ReturnType lsm6d_set_gyro_odr(lsm6d_dev_t* dev, lsm6d_gy_odr_t odr);
Std_ReturnType lsm6d_set_gyro_fs(lsm6d_dev_t* dev, lsm6d_gy_fs_t fs);

/* --- Interrupt configuration --- */
Std_ReturnType lsm6d_set_int1_ctrl(lsm6d_dev_t* dev, lsm6d_int_ctrl_t* cfg);
Std_ReturnType lsm6d_set_int1_event(lsm6d_dev_t* dev, lsm6d_int_event_t* cfg);
Std_ReturnType lsm6d_set_wakeup(lsm6d_dev_t* dev, lsm6d_wakeup_cfg_t* cfg);
Std_ReturnType lsm6d_set_freefall(lsm6d_dev_t* dev, lsm6d_ff_cfg_t* cfg);

/* --- Read individual --- */
Std_ReturnType lsm6d_read_accel(lsm6d_dev_t* dev, Accel_Gyro_t* accel);
Std_ReturnType lsm6d_read_gyro(lsm6d_dev_t* dev, Accel_Gyro_t* gyro);
Std_ReturnType lsm6d_read_temp(lsm6d_dev_t* dev, float* temp);
Std_ReturnType lsm6d_read_all(lsm6d_dev_t* dev, lsm6d_data_t* data);

/* --- Status --- */
Std_ReturnType lsm6d_get_status(lsm6d_dev_t* dev, uint8_t* status);
bool           lsm6d_is_accel_ready(lsm6d_dev_t* dev);
bool           lsm6d_is_gyro_ready(lsm6d_dev_t* dev);

#endif	/* LSM6DSOX_H */

