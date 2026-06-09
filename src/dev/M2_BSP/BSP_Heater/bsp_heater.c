#include "bsp_heater.h"
#include "bsp_core.h"
#include "peripheral/tc/plib_tc0.h"
#include "peripheral/tc/plib_tc1.h"
#include "peripheral/tc/plib_tc2.h"

void bsp_heater_set_duty(heater_channel_t HTR_CH, uint8_t HTR_DUTY)
{
    uint16_t period = 0;
    uint16_t compare = 0;
    switch (HTR_CH)
    {
    case HTR_1:
        period = TC0_CH2_ComparePeriodGet();
        if (HTR_DUTY == 100)
            compare = 1;
        else
            compare = ((100 - HTR_DUTY) * period) / 100;
        TC0_CH2_CompareBSet(compare);
        break;
    case HTR_2:
        period = TC0_CH0_ComparePeriodGet();
        if (HTR_DUTY == 100)
            compare = 1;
        else
            compare = ((100 - HTR_DUTY) * period) / 100;
        TC0_CH0_CompareASet(compare);
        break;
    case HTR_3:
        period = TC0_CH0_ComparePeriodGet();
        if (HTR_DUTY == 100)
            compare = 1;
        else
            compare = ((100 - HTR_DUTY) * period) / 100;
        TC0_CH0_CompareBSet(compare);
        break;
    case HTR_4:
        period = TC0_CH2_ComparePeriodGet();
        if (HTR_DUTY == 100)
            compare = 1;
        else
            compare = ((100 - HTR_DUTY) * period) / 100;
        TC0_CH2_CompareASet(compare);
        break;
    case HTR_5:
        period = TC2_CH1_ComparePeriodGet();
        if (HTR_DUTY == 100)
            compare = 1;
        else
            compare = ((100 - HTR_DUTY) * period) / 100;
        TC2_CH1_CompareASet(compare);
        break;
    case HTR_6:
        period = TC1_CH0_ComparePeriodGet();
        if (HTR_DUTY == 100)
            compare = 1;
        else
            compare = ((100 - HTR_DUTY) * period) / 100;
        TC1_CH0_CompareASet(compare);
        break;
    case HTR_7:
        period = TC2_CH0_ComparePeriodGet();
        if (HTR_DUTY == 100)
            compare = 1;
        else
            compare = ((100 - HTR_DUTY) * period) / 100;
        TC2_CH0_CompareASet(compare);
        break;
    case HTR_8:
        period = TC2_CH0_ComparePeriodGet();
        if (HTR_DUTY == 100)
            compare = 1;
        else
            compare = ((100 - HTR_DUTY) * period) / 100;
        TC2_CH0_CompareBSet(compare);
        break;
    default:
        break;
    }
}

void bsp_heater_off(heater_channel_t HTR_CH)
{
    return;
}
