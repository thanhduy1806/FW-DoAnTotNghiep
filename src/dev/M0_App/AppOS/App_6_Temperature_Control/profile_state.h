#ifndef PROFILE_STATE_H
#define PROFILE_STATE_H

#include <stdint.h>
#include "sw_timer.h"
#include "pid.h"

/* ── Control modes ──────────────────────────────── */
#define HOLD_ON_SETPOINT_MODE 0
#define HEATING_MODE 1
#define COOLING_MODE 2

/* ── Configuration ──────────────────────────────── */
#define PROFILE_MAX_NUM             8
#define PROFILE_MAX_STEP_NUM        10
#define CONTROLLER_SATURATION_TIME  5000U /* ms: thời gian PID output ≈ 0 → coi là đã ổn định */
#define SOAK_TEMP_BAND_C            2.0f            /* ±°C: band để bắt đầu đếm thời gian soak          */
#define TEMP_SCALE                  100.0f                /* 0.01°C → °C                                      */

/* ── Events ─────────────────────────────────────── */
typedef enum {
    PROFILE_EVENT_NONE,
    PROFILE_EVENT_ENABLED,
    PROFILE_EVENT_START,
    PROFILE_EVENT_STOP,
    PROFILE_EVENT_TEMP_ERROR,
    PROFILE_EVENT_TIMER_EXPIRED
} ProfileEvent_e;

/* ── States ─────────────────────────────────────── */
typedef enum {
    PROFILE_STATE_DISABLED,
    PROFILE_STATE_PREHEAT_HEAT,
    PROFILE_STATE_PREHEAT_COOL,
    PROFILE_STATE_HEATING,
    PROFILE_STATE_SOAK_HEAT,
    PROFILE_STATE_SOAK_COOL,
    PROFILE_STATE_COOLING,
    PROFILE_STATE_PROFILE_CHECK,
    PROFILE_STATE_ERROR
} ProfileState_s;

/* ── Step data ──────────────────────────────────── */
typedef struct {
    int16_t start_point; /* 0.01°C: nhiệt độ đầu bước                            */
    int16_t set_point; /* 0.01°C: nhiệt độ mục tiêu                             */
    uint16_t duration; /* giây: thời gian của bước                              */
    uint16_t control_mode; /* SOAK / HEATING_MODE / COOLING_MODE   */
} step_t;

/* ── Profile data ───────────────────────────────── */

/* Chứa thông tin về profile đang chạy */
typedef struct {
    uint8_t enabled; /* 1: profile đang active, 0: dừng                  */
    uint8_t start; /* start the profile steps                          */
    uint8_t main_ntc; /* chỉ số NTC chính                                 */
    uint8_t sec_ntc; /* chỉ số NTC phụ                                   */
    uint8_t tec_mask; /* bitmask: 1 = có TEC, 0 = không TEC               */
    uint8_t heater_mask; /* bitmask: 1 = có Heater, 0 = không Heater         */
    int16_t set_point; /* 0.01°C: setpoint preheat                         */
    int16_t main_sec_delta; /* 0.01°C: chênh lệch tối đa cho phép giữa 2 NTC    */
    step_t steps[PROFILE_MAX_STEP_NUM];
    uint8_t step_count;
} profileData_t;

/* ── State machine context ──────────────────────── */

/* Chứa state hiện tại, dữ liệu profile, thông tin cảm biến, PID, sw_timer... */
typedef struct ProfileState_t {
    ProfileState_s current_state; /* state hiện tại của state machine                */
    profileData_t *data; /* pointer đến profile data (do caller quản lý)    */
    int16_t current_main_temp; /* 0.01°C */
    int16_t current_sec_temp; /* 0.01°C */
    PID_t pid;
    SwTimer_t timer;
    int8_t current_step; /* -1 = chưa bắt đầu */
} ProfileState_t;

/* ── HAL (implement theo board) ─────────────────── */
void update_temperature(ProfileState_t *me);
void heater_set_duty(ProfileState_t *me, uint8_t duty_pct); /* 0–100 % */
void cooler_set_duty(ProfileState_t *me, uint8_t duty_pct); /* 0–100 % */

/* ── Public API ─────────────────────────────────── */
void profileSM_init(ProfileState_t *me, profileData_t *data);
ProfileEvent_e profileSM_get_event(ProfileState_t *me);
void profileSM_handle_event(ProfileState_t *me, ProfileEvent_e event);

PID_instance_t* pid_get(ProfileState_s state);
void pid_set(ProfileState_s state, float kP, float kI, float kD);

#endif /* PROFILE_STATE_H */