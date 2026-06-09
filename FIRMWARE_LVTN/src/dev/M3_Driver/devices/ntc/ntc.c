/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "stdio.h"
#include "stdint.h"
#include <math.h>

#include "ntc.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define _NTC_2152722607
#define _NTC_R_SERIES         10000.0f
#define _NTC_R_NOMINAL        10000.0f
#define _NTC_TEMP_NOMINAL     25.0f
#define _NTC_ADC_MAX          (1 << 24)
#define _NTC_BETA             3950

#ifndef NTC_PULLDOWN_TO_GND
#define NTC_PULLDOWN_TO_GND   0
#endif

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
int16_t ntc_convert_from_adc(uint32_t code24)
{
    // Clamp to avoid div-by-zero at the rails
    if (code24 == 0U)
    {
        code24 = 1U;
    }             
    if (code24 >= (uint32_t)_NTC_ADC_MAX)
    {
        code24 = (uint32_t)_NTC_ADC_MAX - 1U;
    }

    const float FS = _NTC_ADC_MAX;
    const float RS = _NTC_R_SERIES;
    const float R0 = _NTC_R_NOMINAL;
    const float B  = _NTC_BETA;
    const float T0 = _NTC_TEMP_NOMINAL + 273.15f;

    const float N  = (float)code24;

    float Rntc;
#if NTC_PULLDOWN_TO_GND
    // Vref -- RS --+-- ADC
    //               |
    //              NTC
    //               |
    //              GND
    Rntc = RS * (N / (FS - N));
#else
    // Vref -- NTC --+-- ADC
    //                |
    //               RS
    //                |
    //               GND
    Rntc = RS * ((FS - N) / N);
#endif

    // Beta equation
    float invT = (1.0f / T0) + (1.0f / B) * logf(Rntc / R0);
    float Tc   = (1.0f / invT) - 273.15f;

    // Scale to x10 and round
    int16_t out_x10 = (int16_t)lroundf(Tc * 10.0f);
    return out_x10;
}

int16_t ntc_convert_from_volt(float vin_mv, float vref_mv)
{
    // Basic guards
    if (vref_mv <= 0.0f)
    {
        return 0; // or some error code mapping if you prefer
    }

    // Normalize to ratio x = VIN/VREF
    float x = vin_mv / vref_mv;

    // Clamp to avoid divide-by-zero or log of non-positive
    if (x < 1e-6f)
    {
        x = 1e-6f;
    }
    if (x > (1.0f - 1e-6f))
    {
        x = 1.0f - 1e-6f;
    }

    const float RS = _NTC_R_SERIES;
    const float R0 = _NTC_R_NOMINAL;
    const float B  = _NTC_BETA;
    const float T0 = _NTC_TEMP_NOMINAL + 273.15f;

    float Rntc;
    #if NTC_PULLDOWN_TO_GND
    // VREF -- RS --+-- ADC (VIN)
    //               |
    //              NTC
    //               |
    //              GND
    Rntc = RS * (x / (1.0f - x));
    #else
    // VREF -- NTC --+-- ADC (VIN)
    //                |
    //               RS
    //                |
    //               GND
    Rntc = RS * ((1.0f - x) / x);
    #endif

    // Beta equation
    float invT = (1.0f / T0) + (1.0f / B) * logf(Rntc / R0);
    float Tc   = (1.0f / invT) - 273.15f;

    // Scale and round to °C ×10
    return (int16_t)lroundf(Tc * 10.0f);
}   

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */