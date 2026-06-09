/*
 * i2c.c
 *
 *  Created on: Oct 16, 2025
 *      Author: Admin
 */

#include "i2c_io.h"
#include "define.h"
#include "os_hal.h"

uint32_t i2c_io_init(i2c_io_t *me)
{
	return os_lock_init(&me->lock) ? ERROR_OK : ERROR_OK;
}
