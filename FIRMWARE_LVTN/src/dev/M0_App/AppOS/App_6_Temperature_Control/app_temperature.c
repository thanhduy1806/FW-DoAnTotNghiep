#include "app_temperature.h"

#include "M0_App/AppOS/Database/DB_Temperature/db_temperature.h"
#include "M0_App/AppOS/Database/DB_Temperature_Profile/db_temperature_profile.h"
#include "M0_App/AppOS/App_6_Temperature_Control/profile_state.h"

#include "M1_SysApp/xlog/xlog.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"

#include "M2_BSP/BSP_Power/bsp_power.h"
#include "M2_BSP/BSP_Heater/bsp_heater.h"
#include "M2_BSP/BSP_TEC/bsp_tec.h"

bool temp_pid_log_ena = 0;

/*==================== Profile Data ====================*/
profileData_t myProfileData = {
    .enabled = 0,
    .start = 0,
    .main_ntc = NTC_1,
    .sec_ntc = NTC_1,
    .tec_mask = 0x01,
    .heater_mask = 0x03,
    .set_point = 3200,
    .step_count = 4,
    .steps =
    {
        {.start_point = 3200, .set_point = 4000, .duration = 60, .control_mode = HEATING_MODE},
        {.start_point = 4000, .set_point = 4000, .duration = 50, .control_mode = HOLD_ON_SETPOINT_MODE},
        {.start_point = 4000, .set_point = 3000, .duration = 120, .control_mode = COOLING_MODE},
        {.start_point = 3000, .set_point = 3000, .duration = 0, .control_mode = HOLD_ON_SETPOINT_MODE},
    }
};

profileData_t myProfileData_1 = {
    .enabled = 0,
    .start = 0,
    .main_ntc = NTC_1,
    .sec_ntc = NTC_1,
    .tec_mask = 0x00,
    .heater_mask = 0x00,
    .set_point = 4000,
    .step_count = 4,
    .steps =
    {
        {.start_point = 2800, .set_point = 3500, .duration = 60, .control_mode = HEATING_MODE},
        {.start_point = 3500, .set_point = 3500, .duration = 50, .control_mode = HOLD_ON_SETPOINT_MODE},
        {.start_point = 3500, .set_point = 2800, .duration = 40, .control_mode = COOLING_MODE},
        {.start_point = 2800, .set_point = 2800, .duration = 0, .control_mode = HOLD_ON_SETPOINT_MODE},
    }
};

/* ==================== State Machine ====================*/
ProfileState_t myProfileState[8];

/*======================== QUEUE ========================*/
QueueHandle_t temperature_command_queue[8];

/* =================== LOCAL FUNCTION ===================*/


void temperature_init(void) {
    bsp_power_tec_on();

    DB_temp_profile_init();

    for (uint8_t i = 0; i < 8; i++) {
        temperature_command_queue[i] = xQueueCreate(2, sizeof (e_temp_cmd_t));
        myProfileState[i].current_step = -1;
        myProfileState[i].data = DB_temp_profile_get_pointer(i);
    }

    DB_temp_profile_write(0, &myProfileData);
    DB_temp_profile_write(1, &myProfileData_1);
}

const osThreadAttr_t temp_attr = {
    .name = "Temperature",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority = configMAX_PRIORITIES - 3
};

void App_TemperatureTask(void *param) {
    e_temp_cmd_t command;
    for (;;) {
        for (uint8_t i = 0; i < PROFILE_MAX_NUM; i++) {
            if (xQueueReceive(temperature_command_queue[i], &command, 0) == pdPASS) {
                switch (command) {
                    case TEMP_CMD_MODE_AUTO:
                        if (DB_temp_profile_validation(get_UART2_CliPointer()) != ERROR_OK)
                            break;
                        DB_temp_profile_read(i, myProfileState[i].data);
                        profileSM_handle_event(&myProfileState[i], PROFILE_EVENT_ENABLED);
                        if (i==0) temp_pid_log_ena = true;
                        break;
                    case TEMP_CMD_EVENT_START:
                        profileSM_handle_event(&myProfileState[i], PROFILE_EVENT_START);
                        if (i==0) temp_pid_log_ena = true;
                        break;
                    case TEMP_CMD_MODE_MANUAL:
                    case TEMP_CMD_OFF:
                    default:
                        profileSM_handle_event(&myProfileState[i], PROFILE_EVENT_STOP);
                        if (i==0) temp_pid_log_ena = false;
                        break;
                }
            }

            update_temperature(&myProfileState[i]);
            sw_timer_tick(&myProfileState[i].timer);

            if (sw_timer_is_expired(&myProfileState[i].timer)) {
                profileSM_handle_event(&myProfileState[i], PROFILE_EVENT_TIMER_EXPIRED);
            } else {
                profileSM_handle_event(&myProfileState[i], PROFILE_EVENT_NONE);
            }
        }

        if (temp_pid_log_ena) 
        {
            for(uint8_t i = 0; i < PROFILE_MAX_NUM; i++) 
            {
                PID_t *pid = &myProfileState[i].pid;

                int step_idx = myProfileState[i].current_step;

                if (step_idx >= 0 && step_idx < myProfileState[i].data->step_count) 
                {
                    step_t *step = &myProfileState[i].data->steps[step_idx];

                    xlog("PROFILE=%d STEP=%d MODE=%d DUR=%d | PID: SP=%.2f PV=%.2f OUT=%.2f ERR=%.2f\r\n",
                            i,
                            step_idx,
                            step->control_mode,
                            step->duration,
                            pid->setpoint,
                            pid->prev_measurement,
                            pid->output,
                            pid->prev_error);
                } else 
                {
                    xlog("PROFILE=%d STEP=NONE | PID: SP=%.2f PV=%.2f OUT=%.2f ERR=%.2f\r\n",
                            i,
                            pid->setpoint,
                            pid->prev_measurement,
                            pid->output,
                            pid->prev_error);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void update_temperature(ProfileState_t *me) {
    me->current_main_temp = (int16_t) (db_temperature_read(me->data->main_ntc) * 100.0f);
    me->current_sec_temp = (int16_t) (db_temperature_read(me->data->sec_ntc) * 100.0f);
}

void heater_set_duty(ProfileState_t *me, uint8_t duty_pct) {
    uint8_t heater_mask = me->data->heater_mask;
    for (uint8_t i = 0; i < 8; i++) {
        if (heater_mask & (1 << i)) {
            bsp_heater_set_duty((heater_channel_t) (i), duty_pct);
        } else {
            bsp_heater_set_duty((heater_channel_t) (i), 0U);
        }
    }
    return;
}

void cooler_set_duty(ProfileState_t *me, uint8_t duty_pct) {
    uint8_t tec_mask = me->data->tec_mask;
    for (uint8_t i = 0; i < 4; i++) {
        if (tec_mask & (1 << i)) {
            bsp_tec_set_output_voltage_channel(p_tec[i],  (((int64_t)duty_pct) * 25000000));
        } else {
            bsp_tec_set_output_voltage_channel(p_tec[i], 0U);
        }
    }
    return;
}