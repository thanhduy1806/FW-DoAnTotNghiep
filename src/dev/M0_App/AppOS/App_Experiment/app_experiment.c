#include "app_experiment.h"
#include "board.h"
#include "samv71q21b.h"
#include "task.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "packs/ATSAMV71Q21B_DFP/component/spi.h"
#include "config/default/peripheral/tc/plib_tc0.h"
#include "config/default/peripheral/tc/plib_tc3.h"

#include "M2_BSP/BSP_Board/bsp_core.h"
#include "M2_BSP/BSP_Power/bsp_power.h"
#include "M2_BSP/BSP_Photo/bsp_photo.h"
#include "M2_BSP/BSP_PSRAM/bsp_psram.h"

uint32_t psram_addr = 0;

__attribute__((section(".ram_nocache"), aligned(32)))
static adc_buf_t adc_pool[BUF_COUNT];

static adc_buf_t *current_buf;
static uint16_t buf_idx;

QueueHandle_t fram_queue;
QueueHandle_t free_queue;

typedef struct {
    uint8_t channel;
    uint8_t sub_state;
    uint32_t counter_10ms;
    uint32_t pre_10ms_time;
    uint32_t main_10ms_time;
    uint32_t post_10ms_time;
    uint8_t wait_spi_timeout;
    uint32_t dac_code;
} exp_runtime_t;

/*==================== STRUCT DATA ====================*/
exp_profile_t s_exp_profile = {
    .channel = 1,
    .laser_intensity = 100,

    .pre_time_ms = 1000,
    .main_time_ms = 2000,
    .post_time_ms = 1000,

    .sampling_rate_khz = 1,
};

static exp_runtime_t exp_rt;

/*==================== DATA ====================*/
uint32_t volatile photo_spi_count = 0;
uint32_t volatile current_channel = 0;
uint32_t volatile photo_spi_set_count = 0;
uint8_t volatile is_spi_counter_finish = 0;
uint8_t volatile sampling_enable = 0;

/*==================== DEFINE ENUM ====================*/
typedef enum {
    SUB_PRE_PHASE,
    SUB_MAIN_PHASE,
    SUB_POST_PHASE,
    SUB_DONE_CH
} exp_sub_state_t;

/*==================== QUEUE ====================*/
QueueHandle_t experiment_command_queue;

/*==================== PROTOTYPE PRIVATE FUNCTIONS ====================*/
static void exp_setup(void);
static void exp_start(void);
static void adc_flush_last_buffer(void);
void TC3_CH1_My_Handler(TC_TIMER_STATUS status, uintptr_t context);

/*==================== PUBLIC FUNCTIONS ====================*/
void experiment_init(void) {
    TC3_CH1_TimerCallbackRegister(TC3_CH1_My_Handler, 0);

    fram_queue = xQueueCreate(BUF_COUNT, sizeof (adc_buf_t *));
    free_queue = xQueueCreate(BUF_COUNT, sizeof (adc_buf_t *));

    experiment_command_queue = xQueueCreate(5, sizeof (exp_cmd_t));

    for (int i = 0; i < BUF_COUNT; i++) {
        adc_buf_t *buf = &adc_pool[i];
        buf->id = i;
        xQueueSend(free_queue, &buf, 0);
    }

    xQueueReceive(free_queue, &current_buf, portMAX_DELAY);
    buf_idx = 0;
}

void experiment_set_profile(exp_profile_t *exp_profile) {
    s_exp_profile.channel = exp_profile->channel;
    s_exp_profile.pre_time_ms = exp_profile->pre_time_ms;
    s_exp_profile.main_time_ms = exp_profile->main_time_ms;
    s_exp_profile.post_time_ms = exp_profile->post_time_ms;
    s_exp_profile.sampling_rate_khz = exp_profile->sampling_rate_khz;
    s_exp_profile.laser_intensity = exp_profile->laser_intensity;
}

/* ============================================================
 *  Experiment_Task
 * ============================================================ */
const osThreadAttr_t exp_attr = {
    .name = "exp",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority = configMAX_PRIORITIES - 3
};

void App_EXPTask(void *param) {
    exp_cmd_t command = EXP_CMD_NULL;
    for (;;) {
        if (xQueueReceive(experiment_command_queue, &command, 0) == pdPASS) {
            if (command == EXP_CMD_RUN) {
                psram_addr = 0;
                exp_setup();
                exp_start();
            }
            else if (command == EXP_CMD_END) {
                bsp_psram_sw_mcu();
                do_reset(&mcu_pmu_gpio_a);
                uint32_t psram_addr = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/*==================== SETUP ====================*/
static void exp_setup(void) {
    bsp_psram_sw_mcu();
    exp_rt.channel = s_exp_profile.channel;
    exp_rt.counter_10ms = 0;
    exp_rt.pre_10ms_time = s_exp_profile.pre_time_ms / 10;
    exp_rt.main_10ms_time = s_exp_profile.main_time_ms / 10;
    exp_rt.post_10ms_time = s_exp_profile.post_time_ms / 10;
    exp_rt.sub_state = SUB_PRE_PHASE;
    exp_rt.dac_code = (uint8_t) ((((float) s_exp_profile.laser_intensity * 256.0) / 100.0) - 1.0);

    uint16_t exp_rt_period = (120000000 / s_exp_profile.sampling_rate_khz) / 1000.0;
    TC0_CH1_ComparePeriodSet(exp_rt_period);
    TC0_CH1_CompareASet(exp_rt_period * 995 / 1000);

    bsp_power_lp_on();
    photo_spi_count = 0;
    is_spi_counter_finish = 0;
    bsp_laser_int_all_sw_off();
    bsp_photo_all_sw_off();
    bsp_laser_int_set_dac(exp_rt.dac_code);
}

static void exp_start(void) {
    bsp_photo_sw_on(exp_rt.channel);
    sampling_enable = 1;
    bsp_photo_start_sampling();
    TC3_CH1_TimerStart();
}

/*============================================================================
 *This is callback function when program is trigger timer to ADC collection
 *============================================================================*/
void TC0_CH1_Handler(void) {
    uint32_t status = TC0_REGS->TC_CHANNEL[1].TC_SR;
    if (status & TC_SR_CPAS_Msk) {
        ads8329_spi_read_val(&photo_adc_dev);
    }
}

void TC3_CH1_My_Handler(TC_TIMER_STATUS status, uintptr_t context) {
    exp_rt.counter_10ms++;
    switch (exp_rt.sub_state) {
        case SUB_PRE_PHASE:
            if (exp_rt.counter_10ms >= exp_rt.pre_10ms_time) {
                bsp_laser_int_sw_on(exp_rt.channel);
                exp_rt.counter_10ms = 0;
                exp_rt.sub_state = SUB_MAIN_PHASE;
            }
            break;
        case SUB_MAIN_PHASE:
            if (exp_rt.counter_10ms >= exp_rt.main_10ms_time) {
                bsp_laser_int_all_sw_off();
                exp_rt.counter_10ms = 0;
                exp_rt.sub_state = SUB_POST_PHASE;
            }
            break;
        case SUB_POST_PHASE:
            if (exp_rt.counter_10ms >= exp_rt.post_10ms_time) {
                sampling_enable = 0;
                bsp_photo_stop_sampling();
                // adc_flush_last_buffer();
                exp_rt.counter_10ms = 0;
                exp_rt.sub_state = SUB_DONE_CH;
            }
            break;
        case SUB_DONE_CH:
            TC3_CH1_TimerStop();
            bsp_psram_sw_mpu();
            do_set(&mcu_pmu_gpio_a);
            DEBUG_SendString("EXP DONE!\r\n");
            break;
        default:
            break;
    }
}

/*============================================================================
 * This is callback function when program is receive data from Photo board
 *============================================================================*/
void SPI0_Handler(void) {
    uint32_t sr = SPI0_REGS->SPI_SR;

    /* RX ready */
    while (SPI0_REGS->SPI_SR & SPI_SR_RDRF_Msk) {
        if (!sampling_enable) {
            volatile uint32_t dummy = SPI0_REGS->SPI_RDR;
            (void) dummy;
            continue;
        }

        ads8329_spi_irq_callback(&photo_adc_dev);
        current_buf->data[buf_idx++] = photo_adc_dev.raw;

        if (buf_idx >= BUF_SIZE) {
            adc_buf_t *full_buf = current_buf;
            adc_buf_t *new_buf = NULL;

            BaseType_t hpw = pdFALSE;

            if (xQueueReceiveFromISR(free_queue, &new_buf, &hpw) == pdTRUE &&
                    xQueueSendFromISR(fram_queue, &full_buf, &hpw) == pdTRUE) {
                current_buf = new_buf;
                buf_idx = 0;
            } else {
                buf_idx = 0;
            }
            portYIELD_FROM_ISR(hpw);
        }
    }

    /* Overrun */
    if (sr & SPI_SR_OVRES_Msk) {
        volatile uint32_t dummy = SPI0_REGS->SPI_RDR;
        (void) dummy;
    }

    /* Mode fault */
    if (sr & SPI_SR_MODF_Msk) {
        volatile uint32_t dummy = SPI0_REGS->SPI_SR;
        (void) dummy;

        SPI0_REGS->SPI_CR = SPI_CR_SPIEN_Msk;
    }

    /* Underrun */
    if (sr & SPI_SR_UNDES_Msk) {
        volatile uint32_t dummy = SPI0_REGS->SPI_SR;
        (void) dummy;
    }
}

const osThreadAttr_t psram_attr = {
    .name = "psram",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority = configMAX_PRIORITIES - 2
};

void APP_PSRAMTask(void *param) {
    adc_buf_t *recv_buf;
    while (1) {
        if (xQueueReceive(fram_queue, &recv_buf, portMAX_DELAY) == pdPASS) {
            uint32_t size_bytes = BUF_SIZE * 2;
            bsp_psram_write(&g_psram, psram_addr, (uint8_t*) recv_buf->data, size_bytes);
            uint8_t buf[128] = {0};
            // float adc_value = ((recv_buf->data[0] * 4.096 / 65535.0) -0.5) / 330000.0;
            float adc_value = (recv_buf->data[0] * 4.096 / 65535.0);
            snprintf(buf, sizeof(buf), "ADC: %.6f A\r\n", adc_value);
            DEBUG_SendString(buf);
            psram_addr += size_bytes;
            xQueueSend(free_queue, &recv_buf, portMAX_DELAY);
        }
    }
}

void adc_flush_last_buffer(void) {
    if (buf_idx > 0) {
        adc_buf_t *last_buf = current_buf;
        xQueueSend(fram_queue, &last_buf, portMAX_DELAY);
        xQueueReceive(free_queue, &current_buf, portMAX_DELAY);
        buf_idx = 0;
    }
}