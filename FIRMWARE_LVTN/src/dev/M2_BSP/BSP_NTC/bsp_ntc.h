#ifndef BSP_NTC_H
#define	BSP_NTC_H

#include <stdint.h>
#include "../../M3_Driver/components/adc/adc.h"

#define _NTC_R_SERIES         10000.0f
#define _NTC_R_NOMINAL        10000.0f
#define _NTC_TEMP_NOMINAL     25.0f
#define _NTC_BETA             3950
    
void bsp_ntc_get_temp(float *temp);

#endif	/* BSP_NTC_H */