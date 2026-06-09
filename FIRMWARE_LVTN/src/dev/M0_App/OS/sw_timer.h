#ifndef SW_TIMER_H
#define SW_TIMER_H

#include <stdint.h>

/* ── Xử lý tùy biến môi trường (Simulation vs FreeRTOS) ── */
#ifdef SIMULATION_MODE
// Môi trường PC (Mô phỏng)
typedef uint32_t SwTickType_t;
// Khai báo hàm lấy tick ảo (Bạn sẽ viết hàm này trong main_test.c)
uint32_t get_system_tick(void);
#define GET_TICK() get_system_tick()
#define MS_TO_TICKS(ms) (ms) // Giả sử 1 tick = 1 ms
#define TICKS_TO_MS(ticks) (ticks)
#else
// Môi trường nhúng thật (FreeRTOS)
#include "FreeRTOS.h"
#include "task.h"
typedef TickType_t SwTickType_t;
#define GET_TICK() xTaskGetTickCount()
#define MS_TO_TICKS(ms) pdMS_TO_TICKS(ms)
#define TICKS_TO_MS(ticks) ((ticks) * portTICK_PERIOD_MS)
#endif

/* ── Trạng thái timer ───────────────────────────── */
typedef enum
{
    SW_TIMER_STOPPED = 0,
    SW_TIMER_RUNNING,
    SW_TIMER_EXPIRED,
} SwTimerState_e;

/* ── Timer struct ───────────────────────────────── */
typedef struct
{
    SwTickType_t start_tick;   /* tick lúc gọi sw_timer_start()  */
    SwTickType_t period_ticks; /* thời gian timeout               */
    SwTimerState_e state;
} SwTimer_t;

/* ── API ────────────────────────────────────────── */
void sw_timer_start(SwTimer_t *t, uint32_t period_ms);
void sw_timer_stop(SwTimer_t *t);
void sw_timer_restart(SwTimer_t *t);
void sw_timer_tick(SwTimer_t *t); /* gọi mỗi vòng lặp  */

uint8_t sw_timer_is_expired(const SwTimer_t *t);
uint8_t sw_timer_is_running(const SwTimer_t *t);
uint32_t sw_timer_elapsed_ms(const SwTimer_t *t);
uint32_t sw_timer_remaining_ms(const SwTimer_t *t);

#endif /* SW_TIMER_H */