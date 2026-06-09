#ifndef DEVICE_MAX14662_H_
#define DEVICE_MAX14662_H_

#include "i2c_io.h"
#include "M3_Driver/components/adc/adc.h"

#define MAX14662_I2C_ADDRESS_SLAVE_1                            0x4C
#define MAX14662_I2C_ADDRESS_SLAVE_2                            0x4D
#define MAX14662_I2C_ADDRESS_WRITE_REGISTER             0x00  // Địa chỉ để ghi dữ liệu vào MAX14662

typedef enum 
{
    MAX14662_OK      =  0,
    MAX14662_ERR     = -1,
} max14662_err_t;

/* Thiết bị */
typedef struct {
    i2c_io_t   *i2c;                                    /* bus I2C trừu tượng */   
    uint8_t    address_slave;                         /* Địa chỉ I2C của MAX14662 */
} max14662_dev_t;

max14662_err_t max14662_init(max14662_dev_t *dev);
max14662_err_t max14662_read(max14662_dev_t *dev, uint8_t *data);
max14662_err_t max14662_write(max14662_dev_t *dev, uint8_t *data);
// max14662_err_t max14662_set_switch_on(uint8_t switch_number);
// max14662_err_t max14662_set_switch_off(uint8_t switch_number);
// max14662_err_t max14662_set_all_switches_on(void);
// max14662_err_t max14662_set_all_switches_off(void);

#endif /* DEVICE_MAX14662_H_ */
