/*
 * tick.h
 *
 *  Created on: Feb 26, 2025
 *      Author: CAO HIEU
 */

#ifndef M5_UTILS_TICK_TICK_H_
#define M5_UTILS_TICK_TICK_H_

#include "stdint.h"

void TickTimer_IRQHandler(void);
uint32_t Utils_GetTick(void);

#endif /* M5_UTILS_TICK_TICK_H_ */