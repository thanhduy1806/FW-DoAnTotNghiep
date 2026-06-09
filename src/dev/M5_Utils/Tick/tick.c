/*
 * tick.c
 *
 *  Created on: Feb 26, 2025
 *      Author: CAO HIEU
 */
#include "tick.h"

volatile uint32_t LL_Tick = 0;

void TickTimer_IRQHandler(void) {
    LL_Tick++;
}

uint32_t Utils_GetTick(void) {
    return LL_Tick;
}