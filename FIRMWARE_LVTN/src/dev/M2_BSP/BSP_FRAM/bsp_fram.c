#include "bsp_fram.h"

int bsp_fram_write(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length)
{
    return mb85rs2mt_write(dev, address, buffer, length);
}

int bsp_fram_read(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length)
{
    return mb85rs2mt_read(dev, address, buffer, length);
}