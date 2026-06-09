/*
 * wd_tpl5010.h
 *
 *  Created on: Mar 6, 2025
 *      Author: CAO HIEU
 */

#ifndef M3_DEVICES_IO_EXWD_TPL5010_WD_TPL5010_H_
#define M3_DEVICES_IO_EXWD_TPL5010_WD_TPL5010_H_

#include "do.h"

#define HIGH_PERIOD   200
#define LOW_PERIOD    600

typedef enum {
    TPL5010_STATE_LOW = 0,
    TPL5010_STATE_HIGH = 1
} tpl5010_state_t;

bool tpl5010_init(do_t *wd_do);

void tpl5010_feed(do_t *wd_do);

tpl5010_state_t tpl5010_get_state(void);


#endif /* M3_DEVICES_IO_EXWD_TPL5010_WD_TPL5010_H_ */
