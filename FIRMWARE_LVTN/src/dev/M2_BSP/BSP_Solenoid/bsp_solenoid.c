#include "bsp_solenoid.h"
#include "bsp_core.h"

#include "M3_Driver/devices/tca6416/tca6416.h"

extern i2c_io_t i2c2;

tca6416a_t expander = {
    .bus = &i2c2,
    .addr7 = 0x20,
};

void bsp_solenoid_init()
{
    tca6416a_init(&expander, &i2c2, 0x20);
    tca6416a_write_outputs(&expander, 0);
    tca6416a_write_modes(&expander, 0);
}

uint32_t bsp_sol_single_on(uint8_t index)
{
    uint32_t rc;
    rc = tca6416a_pin_write(&expander,index,1);
    return rc;
}

uint32_t bsp_sol_single_off(uint8_t index)
{
    uint32_t rc;
    rc = tca6416a_pin_write(&expander,index,0);
    return rc;
}

uint32_t bsp_sol_single_status(uint8_t index)
{
    uint32_t rc=0;
    rc = tca6416a_pin_read(&expander,index);
    return rc;
}
