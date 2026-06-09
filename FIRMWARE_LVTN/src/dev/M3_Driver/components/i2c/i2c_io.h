/*
 * i2c.h
 *
 *  Created on: Oct 11, 2025
 *      Author: Admin
 */

#ifndef _I2C_IO_H_
#define _I2C_IO_H_
#include <stdint.h>
#include "os_hal.h"


typedef struct i2c_io_t{
	uint32_t 			ui32I2cPort;
	os_lock_t			lock;
}i2c_io_t;

void TWIHS2_Initialize( void );
void TWIHS1_Initialize( void );
void TWIHS0_Initialize( void );

uint32_t i2c_io_init(i2c_io_t *me);
uint32_t i2c_io_enable(i2c_io_t *me);
uint32_t i2c_io_send(struct i2c_io_t *me, uint8_t ui8SlaveAddr, const char *buf, int count);
uint32_t i2c_io_recv(struct i2c_io_t *client, uint8_t ui8SlaveAddr,  char *buf, int count);

#endif /* DRIVERS_COMPONENTS_I2C_I2C_H_ */
