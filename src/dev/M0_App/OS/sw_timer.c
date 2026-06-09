#include "sw_timer.h"
#include <stddef.h>

void sw_timer_start(SwTimer_t *t, uint32_t period_ms) {
    if (t == NULL) return;
    t->period_ticks = MS_TO_TICKS(period_ms);
    t->start_tick = GET_TICK();
    t->state = SW_TIMER_RUNNING;
}

void sw_timer_stop(SwTimer_t *t) {
    if (t == NULL) return;
    t->state = SW_TIMER_STOPPED;
}

void sw_timer_restart(SwTimer_t *t) {
    if (t == NULL) return;
    // Chỉ restart nếu timer chưa bị stop hẳn (hoặc bạn có thể cho phép luôn tùy logic)
    if (t->state != SW_TIMER_STOPPED) {
        t->start_tick = GET_TICK();
        t->state = SW_TIMER_RUNNING;
    }
}

void sw_timer_tick(SwTimer_t *t) {
    if (t == NULL) return;
    
    if (t->state == SW_TIMER_RUNNING) {
        SwTickType_t current_tick = GET_TICK();
        
        // Phép trừ số nguyên không dấu (unsigned) tự động xử lý hiện tượng tràn biến (Overflow)
        if ((current_tick - t->start_tick) >= t->period_ticks) {
            t->state = SW_TIMER_EXPIRED;
        }
    }
}

uint8_t sw_timer_is_expired(const SwTimer_t *t) {
    return (t != NULL && t->state == SW_TIMER_EXPIRED) ? 1 : 0;
}

uint8_t sw_timer_is_running(const SwTimer_t *t) {
    return (t != NULL && t->state == SW_TIMER_RUNNING) ? 1 : 0;
}

uint32_t sw_timer_elapsed_ms(const SwTimer_t *t) {
    if (t == NULL || t->state == SW_TIMER_STOPPED) return 0;
    
    SwTickType_t elapsed_ticks = GET_TICK() - t->start_tick;
    return TICKS_TO_MS(elapsed_ticks);
}

uint32_t sw_timer_remaining_ms(const SwTimer_t *t) {
    if (t == NULL || t->state != SW_TIMER_RUNNING) return 0;
    
    SwTickType_t elapsed_ticks = GET_TICK() - t->start_tick;
    
    if (elapsed_ticks >= t->period_ticks) {
        return 0; // Đã hết hạn
    }
    
    SwTickType_t remaining_ticks = t->period_ticks - elapsed_ticks;
    return TICKS_TO_MS(remaining_ticks);
}