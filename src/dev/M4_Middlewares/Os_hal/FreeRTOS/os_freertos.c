#include "os_hal.h"

#include "FreeRTOS.h"
#include "portmacro.h"
#include "semphr.h"
#include "task.h"
#include <stdint.h>
#include "define.h"

/* =========================================================
 * Internal definition (opaque to users)
 * ========================================================= */
struct os_lock {
    SemaphoreHandle_t h;
};

/* =========================================================
 * Helpers
 * ========================================================= */
static TickType_t ms_to_ticks(uint32_t timeout)
{
    if (timeout == OS_WAIT_FOREVER) {
        return portMAX_DELAY;
    }
    return (TickType_t)pdMS_TO_TICKS(timeout);
}
void os_delay_ms(uint32_t ms)
{
    vTaskDelay(ms_to_ticks(ms));
}
void os_sleep_ms(uint32_t ms)
{
    vTaskDelay(ms_to_ticks(ms));
}
static bool osIsInISR(void)
{
#if defined(xPortIsInsideInterrupt)
    return (xPortIsInsideInterrupt() != pdFALSE);
#elif defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__ICCARM__)
    uint32_t ipsr;
    __asm volatile ("MRS %0, IPSR" : "=r"(ipsr));
    return (ipsr != 0u);
#else
    return false;
#endif
}

/* =========================================================
 * Mutex implementation
 * ========================================================= */
bool os_lock_init(os_lock_t* out)
{
    if (!out) return false;

    struct os_lock* l = pvPortMalloc(sizeof(struct os_lock));
    if (!l) return false;

    l->h = xSemaphoreCreateMutex();   /* priority inheritance */
    if (!l->h) {
        vPortFree(l);
        return false;
    }

    *out = (os_lock_t)l;
    return true;
}

bool os_lock_acquire(os_lock_t lock, uint32_t timeout)
{
    if (!lock) return false;
    if (osIsInISR()) return false;   /* mutex must not be taken in ISR */

    struct os_lock* l = (struct os_lock*)lock;
    return (xSemaphoreTake(l->h, ms_to_ticks(timeout)) == pdTRUE);
}

void os_lock_release(os_lock_t lock)
{
    if (!lock) return;
    if (osIsInISR()) return;

    struct os_lock* l = (struct os_lock*)lock;
    (void)xSemaphoreGive(l->h);
}
uint32_t os_reg_wait_flag(const volatile uint32_t* reg, uint32_t flag, uint32_t timeout_ms)
{
    uint32_t tick = 0;

    while (((*reg) & flag) == 0) {
        os_sleep_ms(1);
        tick++;
        if (tick >= timeout_ms) {
            return ERROR_TIMEOUT;  /* timeout */
        }
    }
    return ERROR_OK;
}

uint32_t os_reg_wait_flag_blocking(const volatile uint32_t* reg, uint32_t flag, uint32_t timeout_ms)
{
    TickType_t start_tick = xTaskGetTickCount();
    while (((*reg) & flag) == 0) {
        if (xTaskGetTickCount() - start_tick >= ms_to_ticks(timeout_ms)) {
            return ERROR_TIMEOUT;  /* timeout */
        }
    }
    return ERROR_OK;
}
uint32_t os_reg_wait_clear_flag_blocking(const volatile uint32_t* reg, uint32_t flag, uint32_t timeout_ms)
{
    TickType_t start_tick = xTaskGetTickCount();
    while (((*reg) & flag) != 0) {
        if (xTaskGetTickCount() - start_tick >= ms_to_ticks(timeout_ms)) {
            return ERROR_TIMEOUT;  /* timeout */
        }
    }
    return ERROR_OK;
}