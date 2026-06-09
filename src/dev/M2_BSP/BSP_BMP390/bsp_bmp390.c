#include "bsp_bmp390.h"
#include "stdbool.h"
#include "bsp_core.h"

bmp390_dev_t bmp390_int =
{
    .i2c_bus  = &i2c1,
    .slaveAdd = 0x76,
    .init_status = false,
};

bmp390_dev_t bmp390_ext =
{
    .i2c_bus  = &i2c0,
    .slaveAdd = 0x77,
    .init_status = false,
};

uint32_t bsp_bmp390_init(bmp390_dev_t* dev)
{
    if(dev == NULL)
        return ERROR_INVALID_PARAM;

    uint32_t status;

    status = i2c_io_init(dev->i2c_bus);
    if(status != ERROR_OK)
        return status;

    status = i2c_io_enable(dev->i2c_bus);
    if(status != ERROR_OK)
        return status;

    return BMP390_init(dev);
}

uint32_t bsp_bmp390_read(bmp390_dev_t* dev, bmp390_data_t* p_data)
{
    return BMP390_read_value(dev, p_data);
}

void bsp_bmp390_ena_switch(void)
{
    do_set(&bmp390_ext_sensor5);
}

void bsp_bmp390_dis_switch(void)
{
    do_reset(&bmp390_ext_sensor5);
}

bool bsp_bmp390_status_switch(void)
{
    return bmp390_ext_sensor5.bStatus;
}
