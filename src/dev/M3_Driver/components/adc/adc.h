#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    AFEC_CH0 = 0U,
    AFEC_CH1,
    AFEC_CH2,
    AFEC_CH3,
    AFEC_CH4,
    AFEC_CH5,
    AFEC_CH6,
    AFEC_CH7,
    AFEC_CH8,
    AFEC_CH9,
    AFEC_CH10,
    AFEC_CH11
}AFEC_CHANNEL_NUM;

#define AFEC_CGR_GAIN_X1   (0x00U)
#define AFEC_CGR_GAIN_X2   (0x01U)
#define AFEC_CGR_GAIN_X4   (0x03U)

#define XDMAC_ACTIVE_CHANNELS_MAX (2U)
#define XDMAC_UBLEN_BIT_WIDTH     (24)
#define ADC_DMA_CH_AFEC0          (0U)
#define ADC_DMA_CH_AFEC1          (1U)
#define AFEC0_SCAN_COUNT          (4U)
#define AFEC1_SCAN_COUNT          (4U)
#define ADC_NTC_COUNT             (8U)
#define ADC_AVG_SAMPLE_COUNT      (10U)

extern uint16_t g_adc_raw[ADC_NTC_COUNT];
extern uint16_t g_adc_avg[ADC_NTC_COUNT];

/* Struct ?? l?u c? TAG và value */
typedef struct {
    uint8_t  ch;   /* Channel number t? TAG */
    uint16_t val;  /* ADC value */
} AFEC_Sample_t;

void AFEC0_Initialize(void);
void AFEC1_Initialize(void);
//void XDMAC_Initialize(void);

void ADC_Read(void);
void ADC_UpdateAverage(void);

uint16_t AFEC0_ReadChannel(uint8_t ch);
uint16_t AFEC1_ReadChannel(uint8_t ch);

uint16_t AFEC0_ReadChannelFromSequence(uint8_t ch);

#endif