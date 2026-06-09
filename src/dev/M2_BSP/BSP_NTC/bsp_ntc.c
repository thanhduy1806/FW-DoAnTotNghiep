#include "bsp_ntc.h"
#include "bsp_core.h"
#include "math.h"

static float ntc_convert_from_volt(float vin_mv, float vref_mv) {
    // Basic guards
    if (vref_mv <= 0.0f) {
        return 0; // or some error code mapping if you prefer
    }

    // Normalize to ratio x = VIN/VREF
    float x = vin_mv / vref_mv;

    // Clamp to avoid divide-by-zero or log of non-positive
    if (x < 1e-6f) {
        x = 1e-6f;
    }
    if (x > (1.0f - 1e-6f)) {
        x = 1.0f - 1e-6f;
    }

    const float RS = _NTC_R_SERIES;
    const float R0 = _NTC_R_NOMINAL;
    const float B = _NTC_BETA;
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
    float Tc = (1.0f / invT) - 273.15f;
    return Tc;
}

void bsp_ntc_get_temp(float *temp) {
    ADC_Read();
    ADC_UpdateAverage();
    for (uint8_t ch = 0; ch < ADC_NTC_COUNT; ch++) {
        float vin_mv = ((float) g_adc_avg[ch] / 4095.0f) * 3300.0f;
        temp[ch] = ntc_convert_from_volt(vin_mv, 3300.0f);
    }
}