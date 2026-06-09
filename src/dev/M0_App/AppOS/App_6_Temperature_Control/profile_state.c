#include "profile_state.h"
#include <math.h>

/* -------------------------------------
 *  PID Configurations
 *
 *  Heating states : out_min=0,    out_max=100  �?? heater_duty = output
 *  Cooling states : out_min=-100, out_max=0    �?? cooler_duty = -output
 *
 *  Lý do dùng output âm cho cooling:
 *    error = setpoint - measurement
 *    Khi cần làm lạnh: setpoint < measurement �?? error < 0
 *    �?? output < 0 �?? cooler_duty = -output > 0  �??
 *    Anti-windup tích hợp vẫn hoạt �?�?ng �?úng chi�?u.
 * ------------------------------------- */
static PID_instance_t PREHEAT_HEAT_PID_CFG = {
    .kp = 60.5f, .ki = 0.08f, .kd = 1.0f, .out_min = 0.0f, .out_max = 100.0f, .dt = 0.1f
};
static PID_instance_t PREHEAT_COOL_PID_CFG = {
    .kp = 50.0f, .ki = 0.08f, .kd = 1.2f, .out_min = -100.0f, .out_max = 0.0f, .dt = 0.1f
};
static PID_instance_t HEATING_PID_CFG = {
    .kp = 60.5f, .ki = 0.08f, .kd = 1.2f, .out_min = 0.0f, .out_max = 100.0f, .dt = 0.1f
};
static PID_instance_t SOAK_HEAT_PID_CFG = {
    .kp = 50.0f, .ki = 0.10f, .kd = 1.0f, .out_min = 0.0f, .out_max = 100.0f, .dt = 0.1f
};
static PID_instance_t SOAK_COOL_PID_CFG = {
    .kp = 50.0f, .ki = 0.10f, .kd = 1.0f, .out_min = -100.0f, .out_max = 0.0f, .dt = 0.1f
};
static PID_instance_t COOLING_PID_CFG = {
    .kp = 100.0f, .ki = 0.10f, .kd = 1.0f, .out_min = -100.0f, .out_max = 0.0f, .dt = 0.1f
};

/* -- Forward declarations ------------------------ */
static void enter_disabled(ProfileState_t *me);
static void enter_preheat(ProfileState_t *me);
static void enter_preheat_heat(ProfileState_t *me);
static void enter_preheat_cool(ProfileState_t *me);
static void enter_heating(ProfileState_t *me);
static void enter_soak_heat(ProfileState_t *me);
static void enter_soak_cool(ProfileState_t *me);
static void enter_cooling(ProfileState_t *me);
static void enter_profile_check(ProfileState_t *me);
static void enter_error(ProfileState_t *me);

static void do_preheat_heat(ProfileState_t *me);
static void do_preheat_cool(ProfileState_t *me);
static void do_heating(ProfileState_t *me);
static void do_soak_heat(ProfileState_t *me);
static void do_soak_cool(ProfileState_t *me);
static void do_cooling(ProfileState_t *me);

/* ------------------------------------------------------------
 *  Helpers
 * ------------------------------------------------------------ */
static inline float main_temp_c(const ProfileState_t *me) {
    return (float) me->current_main_temp / TEMP_SCALE;
}

static void all_outputs_off(ProfileState_t *me) {
    heater_set_duty(me, 0u);
    cooler_set_duty(me, 0u);
}

/* -------------------------------------
 *  Enter functions
 * ------------------------------------- */

static void enter_disabled(ProfileState_t *me) {
    all_outputs_off(me);
    sw_timer_stop(&me->timer);
    me->current_step = -1;
    me->current_state = PROFILE_STATE_DISABLED;
}

static void enter_preheat(ProfileState_t *me) {
    if (me->current_main_temp <= me->data->set_point) {
        enter_preheat_heat(me);
    } else {
        enter_preheat_cool(me);
    }
}

static void enter_preheat_heat(ProfileState_t *me) {
    pid_init(&me->pid, &PREHEAT_HEAT_PID_CFG, main_temp_c(me));
    me->pid.setpoint = (float) me->data->set_point / TEMP_SCALE;
    sw_timer_stop(&me->timer);
    me->current_state = PROFILE_STATE_PREHEAT_HEAT;
}

static void enter_preheat_cool(ProfileState_t *me) {
    pid_init(&me->pid, &PREHEAT_COOL_PID_CFG, main_temp_c(me));
    me->pid.setpoint = (float) me->data->set_point / TEMP_SCALE;
    sw_timer_stop(&me->timer);
    me->current_state = PROFILE_STATE_PREHEAT_COOL;
}

static void enter_heating(ProfileState_t *me) {
    step_t *step = &me->data->steps[me->current_step];
    pid_init(&me->pid, &HEATING_PID_CFG, main_temp_c(me));

    me->pid.setpoint = (float) step->start_point / TEMP_SCALE;
    if (step->duration)
        sw_timer_start(&me->timer, (uint32_t) step->duration * 1000UL);
    me->current_state = PROFILE_STATE_HEATING;
}

static void enter_soak_heat(ProfileState_t *me) {
    step_t *step = &me->data->steps[me->current_step];
    pid_init(&me->pid, &SOAK_HEAT_PID_CFG, main_temp_c(me));
    me->pid.setpoint = (float) step->set_point / TEMP_SCALE;
    sw_timer_stop(&me->timer);
    me->current_state = PROFILE_STATE_SOAK_HEAT;
}

static void enter_soak_cool(ProfileState_t *me) {
    step_t *step = &me->data->steps[me->current_step];
    pid_init(&me->pid, &SOAK_COOL_PID_CFG, main_temp_c(me));
    me->pid.setpoint = (float) step->set_point / TEMP_SCALE;
    sw_timer_stop(&me->timer);
    me->current_state = PROFILE_STATE_SOAK_COOL;
}

static void enter_cooling(ProfileState_t *me) {
    step_t *step = &me->data->steps[me->current_step];
    pid_init(&me->pid, &COOLING_PID_CFG, main_temp_c(me));
    /* Seed setpoint tại start_point; ramp sẽ giảm dần �?ến set_point */
    me->pid.setpoint = (float) step->start_point / TEMP_SCALE;
    if (step->duration)
        sw_timer_start(&me->timer, (uint32_t) step->duration * 1000UL);
    me->current_state = PROFILE_STATE_COOLING;
}

static void enter_profile_check(ProfileState_t *me) {
    me->current_state = PROFILE_STATE_PROFILE_CHECK;
    me->current_step++;

    /* Tất cả bư�?c �?ã hoàn thành �?? kết thúc profile */
    if (me->current_step >= (int8_t) me->data->step_count) {
        me->data->enabled = 0; /* báo hi�?u cho caller: profile hoàn tất */
        enter_disabled(me);
        return;
    }

    step_t *step = &me->data->steps[me->current_step];

    switch (step->control_mode) {
        case HOLD_ON_SETPOINT_MODE:
            if (me->current_main_temp <= step->set_point) {
                enter_soak_heat(me);
            } else {
                enter_soak_cool(me);
            }
            break;
        case HEATING_MODE:
            enter_heating(me);
            break;
        case COOLING_MODE:
            enter_cooling(me);
            break;
        default:
            enter_error(me);
            break;
    }
}

static void enter_error(ProfileState_t *me) {
    all_outputs_off(me);
    sw_timer_stop(&me->timer);
    me->current_state = PROFILE_STATE_ERROR;
}

/* ------------------------------------------------------------
 *  Do functions �?? g�?i m�?i control tick (100 ms)
 * ------------------------------------------------------------ */

/* ------------------------------------------------------------
 *  PREHEAT_HEAT
 *  PID gia nhi�?t �?ến data->set_point.
 *  Khi output bão hòa �? 0 (nhi�?t �?�? �?ạt setpoint),
 *  bắt �?ầu �?ếm CONTROLLER_SATURATION_TIME.
 *  Timer hết �?? PROFILE_CHECK.
 * ----------------------------------------------------------- */
static void do_preheat_heat(ProfileState_t *me) {
    float output = pid_compute(&me->pid, main_temp_c(me));

    heater_set_duty(me, (uint8_t) output);
    cooler_set_duty(me, 0u);

//    if (output < 1.0f) {
//        if (me->timer.state == SW_TIMER_STOPPED) {
//            sw_timer_start(&me->timer, CONTROLLER_SATURATION_TIME);
//        }
//    } else {
//        sw_timer_stop(&me->timer);
//    }
}

/* ------------------------------------------------------------
 *  PREHEAT_COOL
 *  PID làm lạnh �?ến data->set_point.
 *  output �?? [-100, 0]: cooler_duty = -output.
 *  output gần 0 �?? nhi�?t �?�? �?ạt setpoint từ phía trên.
 * ----------------------------------------------------------- */
static void do_preheat_cool(ProfileState_t *me) {
    float output = pid_compute(&me->pid, main_temp_c(me));

    cooler_set_duty(me, (uint8_t) (-output));
    heater_set_duty(me, 0u);

//    if (output > -1.0f) {
//        if (me->timer.state == SW_TIMER_STOPPED) {
//            sw_timer_start(&me->timer, CONTROLLER_SATURATION_TIME);
//        }
//    } else {
//        sw_timer_stop(&me->timer);
//    }
}

/* ------------------------------------------------------------
 *  HEATING (ramp)
 *  Setpoint t�?ng tuyến tính từ start_point �?ến set_point
 *  trong th�?i gian duration giây.
 *  sw_timer �?ược start trong enter_heating; hết timer �?? PROFILE_CHECK.
 * ----------------------------------------------------------- */
static void do_heating(ProfileState_t *me) {
    step_t *step = &me->data->steps[me->current_step];

    float target_temp = (float) step->set_point / TEMP_SCALE;
    float start_temp = (float) step->start_point / TEMP_SCALE;
    float total_change = target_temp - start_temp;
    float ramp_rate_per_dt = (total_change / (float) step->duration) * me->pid.dt;

    me->pid.setpoint += ramp_rate_per_dt;

    /* Clamp cả hai chi�?u tránh sai s�? tích lũy s�? thực */
    if (total_change > 0.0f) {
        if (me->pid.setpoint > target_temp)
            me->pid.setpoint = target_temp;
    } else if (total_change < 0.0f) {
        if (me->pid.setpoint < target_temp)
            me->pid.setpoint = target_temp;
    }

    float output = pid_compute(&me->pid, main_temp_c(me));

    heater_set_duty(me, (uint8_t) output);
    cooler_set_duty(me, 0u);
}

/* ------------------------------------------------------------
 *  SOAK_HEAT
 *  PID giữ nhi�?t tại step->set_point.
 *  Timer ch�? bắt �?ầu �?ếm khi nhi�?t �?�? vào band ±SOAK_TEMP_BAND_C.
 *  Nếu nhi�?t �?�? r�?i band �?? timer reset (yêu cầu giữ liên tục).
 * ----------------------------------------------------------- */
static void do_soak_heat(ProfileState_t *me) {
    step_t *step = &me->data->steps[me->current_step];
    float measurement = main_temp_c(me);
    float setpoint = (float) step->set_point / TEMP_SCALE;
    float output = pid_compute(&me->pid, measurement);

    heater_set_duty(me, (uint8_t) output);
    cooler_set_duty(me, 0u);

    if (fabsf(measurement - setpoint) <= SOAK_TEMP_BAND_C) {
        if (me->timer.state == SW_TIMER_STOPPED) {
            if (step->duration)
                sw_timer_start(&me->timer, (uint32_t) step->duration * 1000UL);
        }
    } else {
        /* R�?i band �?? reset: yêu cầu giữ nhi�?t LI�?N TỤC �?ủ duration */
        if (me->timer.state == SW_TIMER_RUNNING) {
            sw_timer_stop(&me->timer);
        }
    }
}

/* ------------------------------------------------------------
 *  SOAK_COOL
 *  Tương tự SOAK_HEAT nhưng dùng cooler.
 *  output �?? [-100, 0]: cooler_duty = -output.
 * ----------------------------------------------------------- */
static void do_soak_cool(ProfileState_t *me) {
    step_t *step = &me->data->steps[me->current_step];
    float measurement = main_temp_c(me);
    float setpoint = (float) step->set_point / TEMP_SCALE;
    float output = pid_compute(&me->pid, measurement);

    cooler_set_duty(me, (uint8_t) (-output));
    heater_set_duty(me, 0u);

    if (fabsf(measurement - setpoint) <= SOAK_TEMP_BAND_C) {
        if (me->timer.state == SW_TIMER_STOPPED) {
            if (step->duration)
                sw_timer_start(&me->timer, (uint32_t) step->duration * 1000UL);
        }
    } else {
        if (me->timer.state == SW_TIMER_RUNNING) {
            sw_timer_stop(&me->timer);
        }
    }
}

/* ------------------------------------------------------------
 *  COOLING (ramp down)
 *  Setpoint giảm tuyến tính từ start_point xu�?ng set_point
 *  trong th�?i gian duration giây. Dùng cooler.
 *  output �?? [-100, 0]: cooler_duty = -output.
 * ----------------------------------------------------------- */
static void do_cooling(ProfileState_t *me) {
    step_t *step = &me->data->steps[me->current_step];

    float target_temp = (float) step->set_point / TEMP_SCALE;
    float start_temp = (float) step->start_point / TEMP_SCALE;
    float total_change = target_temp - start_temp; /* âm v�?i cooling */
    float ramp_rate_per_dt = (total_change / (float) step->duration) * me->pid.dt;

    me->pid.setpoint += ramp_rate_per_dt;

    /* Clamp */
    if (total_change < 0.0f) {
        if (me->pid.setpoint < target_temp)
            me->pid.setpoint = target_temp;
    } else if (total_change > 0.0f) {
        if (me->pid.setpoint > target_temp)
            me->pid.setpoint = target_temp;
    }

    float output = pid_compute(&me->pid, main_temp_c(me));

    cooler_set_duty(me, (uint8_t) (-output));
    heater_set_duty(me, 0u);
}

/* ------------------------------------------------------------
 *  HAL placeholders �?? implement theo board
 * ----------------------------------------------------------- */
__attribute__((weak)) void update_temperature(ProfileState_t *me) {
    me->current_main_temp = 0;
    me->current_sec_temp = 0;
}

__attribute__((weak)) void heater_set_duty(ProfileState_t *me, uint8_t duty_pct) {
    (void) me;
    (void) duty_pct;
    /* TODO: TIM->CCR = duty_pct * (TIM->ARR + 1) / 100 */
}

__attribute__((weak)) void cooler_set_duty(ProfileState_t *me, uint8_t duty_pct) {
    (void) me;
    (void) duty_pct;
    /* TODO: TEC PWM hoặc fan speed */
}

/* ------------------------------------------------------------
 *  Public API
 * ----------------------------------------------------------- */

void profileSM_init(ProfileState_t *me, profileData_t *data) {
    me->current_state = PROFILE_STATE_DISABLED;
    me->data = data;
    me->current_step = -1;
    sw_timer_stop(&me->timer);
    all_outputs_off(me);
}

/* ------------------------------------------------------------
 *  profileSM_get_event
 *  G�?i m�?i control tick TRƯ�?C handle_event.
 *  �?�?c cảm biến, cập nhật sw_timer, trả v�? event.
 * ----------------------------------------------------------- */
ProfileEvent_e profileSM_get_event(ProfileState_t *me) {

    /* Stop request từ bên ngoài */
    if (!me->data->enabled) {
        return PROFILE_EVENT_STOP;
    }

    /* DISABLED: ch�? tín hi�?u enable */
    if (me->current_state == PROFILE_STATE_DISABLED) {
        return (me->data->enabled) ? PROFILE_EVENT_ENABLED : PROFILE_EVENT_NONE;
    }

    if ((me->current_state == PROFILE_STATE_PREHEAT_HEAT) || (me->current_state == PROFILE_STATE_PREHEAT_COOL)) {
        return (me->data->start) ? PROFILE_EVENT_START : PROFILE_EVENT_NONE;
    }

    /* �?�?c nhi�?t �?�? */
    update_temperature(me);

    /* Ki�?m tra chênh l�?ch 2 cảm biến */
    int16_t delta = me->current_main_temp - me->current_sec_temp;
    if (delta < 0)
        delta = (int16_t) - delta;
    if (delta > me->data->main_sec_delta) {
        return PROFILE_EVENT_TEMP_ERROR;
    }

    /* Cập nhật sw_timer */
    sw_timer_tick(&me->timer);
    if (me->timer.state == SW_TIMER_EXPIRED) {
        sw_timer_stop(&me->timer);
        return PROFILE_EVENT_TIMER_EXPIRED;
    }

    return PROFILE_EVENT_NONE;
}

/* ------------------------------------------------------------
 *  profileSM_handle_event
 *  Xử lý transition và g�?i do_* function.
 * ----------------------------------------------------------- */
void profileSM_handle_event(ProfileState_t *me, ProfileEvent_e event) {
    if (event == PROFILE_EVENT_STOP) {
        enter_disabled(me);
        return;
    }

    if (event == PROFILE_EVENT_TEMP_ERROR &&
            me->current_state != PROFILE_STATE_DISABLED &&
            me->current_state != PROFILE_STATE_ERROR) {
        enter_error(me);
        return;
    }

    switch (me->current_state) {
            /* -- DISABLED -- */
        case PROFILE_STATE_DISABLED:
            if (event == PROFILE_EVENT_ENABLED) {
                enter_preheat(me);
            }
            break;

            /* -- PREHEAT_HEAT -- */
        case PROFILE_STATE_PREHEAT_HEAT:
            if (event == PROFILE_EVENT_START || event == PROFILE_EVENT_TIMER_EXPIRED) {
                enter_profile_check(me);
            } else {
                do_preheat_heat(me);
            }
            break;

            /* -- PREHEAT_COOL -- */
        case PROFILE_STATE_PREHEAT_COOL:
            if (event == PROFILE_EVENT_START || event == PROFILE_EVENT_TIMER_EXPIRED) {
                enter_profile_check(me);
            } else {
                do_preheat_cool(me);
            }
            break;

            /* -- HEATING -- */
        case PROFILE_STATE_HEATING:
            if (event == PROFILE_EVENT_TIMER_EXPIRED) {
                enter_profile_check(me);
            } else {
                do_heating(me);
            }
            break;

            /* -- SOAK_HEAT -- */
        case PROFILE_STATE_SOAK_HEAT:
            if (event == PROFILE_EVENT_TIMER_EXPIRED) {
                enter_profile_check(me);
            } else {
                do_soak_heat(me);
            }
            break;

            /* -- SOAK_COOL -- */
        case PROFILE_STATE_SOAK_COOL:
            if (event == PROFILE_EVENT_TIMER_EXPIRED) {
                enter_profile_check(me);
            } else {
                do_soak_cool(me);
            }
            break;

            /* -- COOLING -- */
        case PROFILE_STATE_COOLING:
            if (event == PROFILE_EVENT_TIMER_EXPIRED) {
                enter_profile_check(me);
            } else {
                do_cooling(me);
            }
            break;

            /* -- PROFILE_CHECK -- */
            /* Transient: enter_profile_check �?ã chuy�?n state  */
        case PROFILE_STATE_PROFILE_CHECK:
            break;

            /* -- ERROR -- */
            /* Ch�? operator tắt enabled và bật lại */
        case PROFILE_STATE_ERROR:
            all_outputs_off(me);
            break;

        default:
            break;
    }
}

PID_instance_t* pid_get(ProfileState_s state) {
    switch (state) {
        case PROFILE_STATE_PREHEAT_HEAT: return &PREHEAT_HEAT_PID_CFG;
        case PROFILE_STATE_PREHEAT_COOL: return &PREHEAT_COOL_PID_CFG;
        case PROFILE_STATE_HEATING: return &HEATING_PID_CFG;
        case PROFILE_STATE_SOAK_HEAT: return &SOAK_HEAT_PID_CFG;
        case PROFILE_STATE_SOAK_COOL: return &SOAK_COOL_PID_CFG;
        case PROFILE_STATE_COOLING: return &COOLING_PID_CFG;

        default: return NULL;
    }
}

void pid_set(ProfileState_s state, float kP, float kI, float kD) {
    PID_instance_t *pid = pid_get(state);

    if (pid == NULL) {
        return;
    }

    if (kP < 0 || kI < 0 || kD < 0) {
        return;
    }

    pid->kp = kP;
    pid->ki = kI;
    pid->kd = kD;
}