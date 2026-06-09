#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_WAIT_FOREVER   (0xFFFFFFFFu)
#define OS_NO_WAIT        (0u)

/* Opaque lock handle */
typedef struct os_lock* os_lock_t;

/* Mutex API */
bool os_lock_init(os_lock_t* out);
bool os_lock_acquire(os_lock_t lock, uint32_t timeout);
void os_lock_release(os_lock_t lock);
void os_delay_ms(uint32_t ms);
void os_sleep_ms(uint32_t ms);
uint32_t os_reg_wait_flag(const volatile uint32_t* reg, uint32_t flag, uint32_t timeout_ms);
uint32_t os_reg_wait_flag_blocking(const volatile uint32_t* reg, uint32_t flag,uint32_t timeout_ms);
uint32_t os_reg_wait_clear_flag_blocking(const volatile uint32_t* reg, uint32_t flag, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
