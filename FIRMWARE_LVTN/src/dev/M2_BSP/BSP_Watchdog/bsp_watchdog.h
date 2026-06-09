#ifndef BSP_WATCHDOG_H_
#define BSP_WATCHDOG_H_

#include "../../M3_Driver/devices/TPL5010/tpl5010.h"
#include "../BSP_Board/bsp_core.h"

#define ALIVE_PERIOD_MS     100U  
#define WD_HIGH_PERIOD_MS   HIGH_PERIOD 
#define WD_LOW_PERIOD_MS    LOW_PERIOD 

void bsp_watchdog_init(void);

void bsp_watchdog_update(uint32_t now);

#endif /* BSP_WATCHDOG_H_ */