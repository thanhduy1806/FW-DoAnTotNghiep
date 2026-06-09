/*
 * spi_io.c
 *
 *  Created on: Oct 11, 2025
 *      Author: Admin
 */

#include "spi_io.h"
#include "os_hal.h"
#include "define.h"
uint32_t spi_io_init(spi_io_t *me)
{
	return os_lock_init(&me->lock) ? ERROR_OK : ERROR_FAIL;

}

