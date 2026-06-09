/*
 * do.h
 *
 *  Created on: Aug 3, 2025
 *      Author: Admin
 */

#ifndef DRIVER_COMPONENTS_DIO_DO_H_
#define DRIVER_COMPONENTS_DIO_DO_H_
#include <stdint.h>
#include "stdbool.h"


typedef struct
{
	uint32_t	 	port;
	uint32_t		pin;
	bool			bStatus;
}do_t;



void do_set(do_t *me);
void do_reset(do_t *me);
void do_toggle(do_t *me);


#endif /* DRIVER_COMPONENTS_DIO_DO_H_ */
