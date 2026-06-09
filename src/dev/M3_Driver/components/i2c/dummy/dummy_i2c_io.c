#include "i2c_io.h"
#include "define.h"

uint32_t i2c_io_recv(struct i2c_io_t *client, uint8_t ui8SlaveAddr,  char *buf, int count)
{
	/* Dummy implementation: Simply return E_OK without doing anything */
	(void)client;
	(void)ui8SlaveAddr;
	(void)buf;
	(void)count;
	return ERROR_OK;
}

uint32_t i2c_io_send(struct i2c_io_t *me, uint8_t ui8SlaveAddr, const char *buf, int count)
{
	/* Dummy implementation: Simply return ERROR_OK without doing anything */
	(void)me;
	(void)ui8SlaveAddr;
	(void)buf;
	(void)count;
	return ERROR_OK;
}	


