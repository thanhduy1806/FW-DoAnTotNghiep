#include "MAX14662.h"
#include "define.h"

max14662_err_t max14662_init(max14662_dev_t *dev)
{
    if (i2c_io_enable(dev->i2c) != ERROR_OK)
    {
        return MAX14662_ERR;
    }
    // return i2c_io_init(i2c) == 0 ? MAX14662_OK : MAX14662_ERR;
    return MAX14662_OK;
}

max14662_err_t max14662_read(max14662_dev_t *dev, uint8_t *data)
{
    if(i2c_io_recv(dev->i2c, dev->address_slave, data, 1) != ERROR_OK)
    {
        return MAX14662_ERR;
    }
    return MAX14662_OK;
}

max14662_err_t max14662_write(max14662_dev_t *dev, uint8_t *data)
{
    uint8_t temp_data[2] = {MAX14662_I2C_ADDRESS_WRITE_REGISTER, *data};
    if(i2c_io_send(dev->i2c, dev->address_slave, temp_data, 2) != ERROR_OK)
    {
        return MAX14662_ERR;
    }
    
    return MAX14662_OK;
}

// max14662_err_t max14662_set_switch_on(uint8_t switch_number)
// {
//     if(switch_number < 1 || switch_number > 8) return MAX14662_ERR; // Chỉ có 8 switch (1-8)

//     uint8_t data;
//     if(max14662_read(NULL, &data) != MAX14662_OK) return MAX14662_ERR;
    
//     data |= (1 << (switch_number - 1)); // Bật switch tương ứng
//     if(max14662_write(NULL, &data) != MAX14662_OK) return MAX14662_ERR;
    
//     return MAX14662_OK;
// }

// max14662_err_t max14662_set_switch_off(uint8_t switch_number)
// {
//     if(switch_number < 1 || switch_number > 8) return MAX14662_ERR; // Chỉ có 8 switch (1-8)

//     uint8_t data;
//     if(max14662_read(NULL, &data) != MAX14662_OK) return MAX14662_ERR;
    
//     data &= ~(1 << (switch_number - 1)); // Tắt switch tương ứng
//     if(max14662_write(NULL, &data) != MAX14662_OK) return MAX14662_ERR;
    
//     return MAX14662_OK;
// }

// max14662_err_t max14662_set_all_switches_on(void)
// {
//     uint8_t data = 0xFF; // Bật tất cả switch (1-8)
//     if(max14662_write(NULL, &data) != MAX14662_OK) return MAX14662_ERR;
    
//     return MAX14662_OK;
// }

// max14662_err_t max14662_set_all_switches_off(void)
// {
//     uint8_t data = 0x00; // Tắt tất cả switch (1-8)
//     if(max14662_write(NULL, &data) != MAX14662_OK) return MAX14662_ERR;
    
//     return MAX14662_OK;
// }





