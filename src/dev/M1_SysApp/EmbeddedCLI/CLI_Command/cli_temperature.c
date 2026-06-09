#include "cli_temperature.h"
#include "cli_command.h"
#include "stdio.h"
#include "stdlib.h"
#include "os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "M0_App/AppOS/Database/DB_Temperature_Profile/db_temperature_profile.h"
#include "M0_App/AppOS/App_6_Temperature_Control/app_temperature.h"
#include "M0_App/AppOS/App_6_Temperature_Control/profile_state.h"
#include "M0_App/AppOS/App_3_CLI/app_cli.h"

#include "M2_BSP/UART/uart_irq.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
//=======================================HUYNH THANH SANG============================================//
///////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t profile_config_index = 0;
static uint8_t current_step_idx = 0;
static profileData_t profileDataTemp;

void CMD_Temp_profile_set(EmbeddedCli *cli, char *args, void *context) {
    app_cli_wizard_start(MAX_STEP_GETTING);

}

void TempProfile_InputHandler(EmbeddedCli *cli, char *input, uint8_t step) {
    switch (step) {
        case GET_CONFIRM:
        {
            UART2_SendString("\r\nStart getting profile...\r\n");
            UART2_SendString(" | -> profile index: ");
            break;
        }

        case GET_PROFILE_INDEX:
        {
            profile_config_index = (uint8_t) atoi(input);
            UART2_SendString("\r\n | Notes: 0=NTC1 1=NTC2 ... 7=NTC8");
            UART2_SendString("\r\n | -> main ntc: ");
            break;
        }

        case GET_MAIN_NTC:
        {
            profileDataTemp.main_ntc = (uint8_t) atoi(input);
            UART2_SendString("\r\n | -> sec ntc: ");
            break;
        }

        case GET_SEC_NTC:
        {
            profileDataTemp.sec_ntc = (uint8_t) atoi(input);
            UART2_SendString("\r\n | Notes: eg:0x01 mean tec1 is ena");
            UART2_SendString("\r\n | -> tec mask: ");
            break;
        }

        case GET_TEC_MASK:
        {
            profileDataTemp.tec_mask = (uint8_t) atoi(input);
            UART2_SendString("\r\n | Notes: eg:0x10 mean heater5 is ena");
            UART2_SendString("\r\n | -> heater mask: ");
            break;
        }

        case GET_HEATER_MASK:
        {
            profileDataTemp.heater_mask = (uint8_t) atoi(input);
            UART2_SendString("\r\n | -> setpoint (0.01*C): ");
            break;
        }

        case GET_SETPOINT:
        {
            profileDataTemp.set_point = (uint16_t) atoi(input);
            UART2_SendString("\r\n | -> main-sec delta (0.01*C): ");
            break;
        }

        case GET_MAIN_SEC_DELTA:
        {
            profileDataTemp.main_sec_delta = (uint16_t) atoi(input);
            UART2_SendString("\r\n | -> step count: ");
            break;
        }

        case GET_STEP_COUNT:
        {
            profileDataTemp.step_count = (uint8_t) atoi(input);

            if (profileDataTemp.step_count == 0 || profileDataTemp.step_count > 8) {
                UART2_SendString("\r\n | Invalid step count: ");
                return;
            }

            current_step_idx = 0;
            UART2_SendString("\r\n | Notes: Format: start(0.01*C) stop(0.01*C) duration(s) mode[0=SOAK 1=HEAT 2=COOL]");
            UART2_SendString("\r\n | -> step[0]: ");
            break;
        }

        case GET_STEP:
        {
            int start, set, duration, mode;

            // parse input
            if (sscanf(input, "%d %d %d %d", &start, &set, &duration, &mode) != 4) {
                UART2_SendString("\r\n | Invalid format! Use: start set duration mode");
                UART2_SendString("\r\n | -> step[");
                char buf[4];
                sprintf(buf, "%d", current_step_idx);
                UART2_SendString(buf);
                UART2_SendString("]: ");
                return;
            }

            // asign input value into profileDataTemp
            profileDataTemp.steps[current_step_idx].start_point = start;
            profileDataTemp.steps[current_step_idx].set_point = set;
            profileDataTemp.steps[current_step_idx].duration = duration;
            profileDataTemp.steps[current_step_idx].control_mode = mode;


            // next step
            current_step_idx++;
            if (current_step_idx < profileDataTemp.step_count) {
                char buf[32];
                sprintf(buf, "\r\n | -> step[%d]: ", current_step_idx);
                UART2_SendString(buf);
            } else {
                // xong, sang SAVE
                UART2_SendString("\r\n Save? (Y/N): ");
            }
            break;
        }

            // ...
        case GET_SAVE:
            DB_temp_profile_write(profile_config_index, &profileDataTemp);
            break;

        default:
            break;
    }
}

uint8_t TempProfile_CheckRemainStep(void) {
    return profileDataTemp.step_count - current_step_idx;
}

void CMD_DB_TempProfile_display(EmbeddedCli *cli, char *args, void *context) {
    const char *token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "Usage: temp_profile_display <index>");
        return;
    }

    int profile_index = atoi(token);
    DB_temp_profile_display(cli, profile_index);
}

void CMD_DB_TempProfile_validation(EmbeddedCli *cli, char *args, void *context) {
    DB_temp_profile_validation(cli);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//=======================================HUYNH THANH SANG============================================//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void CMD_Temp_auto_ena(EmbeddedCli *cli, char *args, void *context) {
    e_temp_cmd_t cmd = TEMP_CMD_MODE_AUTO;

    // Parse tham so
    const char *token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "Usage: temp_auto_ena <index>");
        return;
    }

    int state_index = atoi(token);

    // Check index hop ly (define max)
    if (state_index < 0 || state_index >= PROFILE_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    if (temperature_command_queue[state_index] == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(temperature_command_queue[state_index], &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Auto mode enabled");
    } else {
        embeddedCliPrint(cli, "Queue full");
    }
}

void CMD_Temp_auto_start(EmbeddedCli *cli, char *args, void *context) {
    e_temp_cmd_t cmd = TEMP_CMD_EVENT_START;

    // Parse tham so
    const char *token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "Usage: temp_auto_start <index>");
        return;
    }

    int state_index = atoi(token);

    if (state_index < 0 || state_index >= PROFILE_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    if (temperature_command_queue[state_index] == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(temperature_command_queue[state_index], &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Start OK");
    } else {
        embeddedCliPrint(cli, "Queue full");
    }
}

void CMD_Temp_manu(EmbeddedCli *cli, char *args, void *context) {
    e_temp_cmd_t cmd = TEMP_CMD_MODE_MANUAL;

    // Parse tham so
    const char *token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "Usage: temp_auto_stop <index>");
        return;
    }

    int state_index = atoi(token);

    if (state_index < 0 || state_index >= PROFILE_MAX_NUM) {
        embeddedCliPrint(cli, "Invalid index");
        return;
    }

    if (temperature_command_queue[state_index] == NULL) {
        embeddedCliPrint(cli, "Queue not init");
        return;
    }

    if (xQueueSend(temperature_command_queue[state_index], &cmd, 0) == pdPASS) {
        embeddedCliPrint(cli, "Stop OK");
    } else {
        embeddedCliPrint(cli, "Queue full");
    }
}

void CMD_Temp_log_toggle(EmbeddedCli *cli, char *args, void *context) {
    temp_pid_log_ena = !((bool) temp_pid_log_ena);
}

void CMD_Temp_PID_set(EmbeddedCli *cli, char *args, void *context) {
    const char *token;
    int state_id;
    float kP, kI, kD;

    // state
    token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "Usage: temp_pid_set <state> <kP> <kI> <kD>");
        return;
    }
    state_id = atoi(token);

    if (state_id >= 7 || state_id <= 0) {
        embeddedCliPrint(cli, "[ERROR] Invalid state");
        embeddedCliPrint(cli, "[NOTES] state = 1 : STATE_PREHEAT_HEAT");
        embeddedCliPrint(cli, "[NOTES] state = 2 : STATE_PREHEAT_COOL");
        embeddedCliPrint(cli, "[NOTES] state = 3 : STATE_HEATING");
        embeddedCliPrint(cli, "[NOTES] state = 4 : STATE_SOAK_HEAT");
        embeddedCliPrint(cli, "[NOTES] state = 5 : STATE_SOAK_COOL");
        embeddedCliPrint(cli, "[NOTES] state = 6 : STATE_COOLING");
        return;
    }

    // kP
    token = embeddedCliGetToken(args, 2);
    if (token == NULL) {
        embeddedCliPrint(cli, "[ERROR] Missing kP");
        return;
    }
    kP = atof(token);

    // kI
    token = embeddedCliGetToken(args, 3);
    if (token == NULL) {
        embeddedCliPrint(cli, "[ERROR] Missing kI");
        return;
    }
    kI = atof(token);

    // kD
    token = embeddedCliGetToken(args, 4);
    if (token == NULL) {
        embeddedCliPrint(cli, "[ERROR] Missing kD");
        return;
    }
    kD = atof(token);

    // validate
    if (kP < 0 || kI < 0 || kD < 0) {
        embeddedCliPrint(cli, "[ERROR] PID must be >= 0");
        return;
    }

    // apply
    pid_set((ProfileState_s) state_id, kP, kI, kD);

    char buf[128];
    snprintf(buf, sizeof (buf),
            "[OK] State %d PID set: Kp=%.3f Ki=%.3f Kd=%.3f",
            state_id, kP, kI, kD);
    embeddedCliPrint(cli, buf);
}

void CMD_Temp_PID_get(EmbeddedCli *cli, char *args, void *context) {
    const char *token;
    token = embeddedCliGetToken(args, 1);
    if (token == NULL) {
        embeddedCliPrint(cli, "Usage: temp_pid_get <state>");
        return;
    }
    int state_id = atoi(token);

    if (state_id >= 7 || state_id <= 0) {
        embeddedCliPrint(cli, "[ERROR] Invalid state");
        embeddedCliPrint(cli, "[NOTES] state = 1 : STATE_PREHEAT_HEAT");
        embeddedCliPrint(cli, "[NOTES] state = 2 : STATE_PREHEAT_COOL");
        embeddedCliPrint(cli, "[NOTES] state = 3 : STATE_HEATING");
        embeddedCliPrint(cli, "[NOTES] state = 4 : STATE_SOAK_HEAT");
        embeddedCliPrint(cli, "[NOTES] state = 5 : STATE_SOAK_COOL");
        embeddedCliPrint(cli, "[NOTES] state = 6 : STATE_COOLING");
        return;
    }

    PID_instance_t *cfg = pid_get((ProfileState_s) state_id);
    if (cfg == NULL) {
        embeddedCliPrint(cli, "[ERROR] No PID config for this state");
        return;
    }

    char buf[128];
    snprintf(buf, sizeof (buf), "State %d PID: Kp=%.3f Ki=%.3f Kd=%.3f", state_id, cfg->kp, cfg->ki, cfg->kd);
    embeddedCliPrint(cli, buf);
}

void CMD_Temp_prof_set(EmbeddedCli *cli, char *args, void *context)
{
    (void)context;

    const char *token;
    profileData_t profileDataTemp = {0};

    int prof_id;
    int arg_idx = 1;

    token = embeddedCliGetToken(args, arg_idx++);
    if (token == NULL) goto error;
    prof_id = atoi(token);

    token = embeddedCliGetToken(args, arg_idx++);
    if (token == NULL) goto error;
    profileDataTemp.main_ntc = (uint8_t)atoi(token);

    token = embeddedCliGetToken(args, arg_idx++);
    if (token == NULL) goto error;
    profileDataTemp.sec_ntc = (uint8_t)atoi(token);

    token = embeddedCliGetToken(args, arg_idx++);
    if (token == NULL) goto error;
    profileDataTemp.tec_mask = (uint8_t)atoi(token);

    token = embeddedCliGetToken(args, arg_idx++);
    if (token == NULL) goto error;
    profileDataTemp.heater_mask = (uint8_t)atoi(token);

    token = embeddedCliGetToken(args, arg_idx++);
    if (token == NULL) goto error;
    profileDataTemp.set_point = (int16_t)atoi(token);

    token = embeddedCliGetToken(args, arg_idx++);
    if (token == NULL) goto error;
    profileDataTemp.main_sec_delta = (int16_t)atoi(token);
    
    token = embeddedCliGetToken(args, arg_idx++);
    if (token == NULL) goto error;
    profileDataTemp.step_count = (uint8_t)atoi(token);

    profileDataTemp.enabled = 0;
    profileDataTemp.start = 0;

    for (uint8_t i = 0; i < profileDataTemp.step_count; i++)
    {
        int start, stop, duration, mode;

        token = embeddedCliGetToken(args, arg_idx++);
        if (token == NULL) break;
        start = atoi(token);

        token = embeddedCliGetToken(args, arg_idx++);
        if (token == NULL) goto error;
        stop = atoi(token);

        token = embeddedCliGetToken(args, arg_idx++);
        if (token == NULL) goto error;
        duration = atoi(token);

        token = embeddedCliGetToken(args, arg_idx++);
        if (token == NULL) goto error;
        mode = atoi(token);

        profileDataTemp.steps[i].start_point = (int16_t)start;
        profileDataTemp.steps[i].set_point = (int16_t)stop;
        profileDataTemp.steps[i].duration = (uint32_t)duration;
        profileDataTemp.steps[i].control_mode = (uint8_t)mode;
    }

    if (prof_id < 0 || prof_id >= PROFILE_MAX_NUM)
    {
        embeddedCliPrint(cli, "[ERROR] invalid profile id");
        return;
    }

    DB_temp_profile_write((uint8_t)prof_id, &profileDataTemp);
    embeddedCliPrint(cli, "[OK] temp_profile_set");
    return;

error:
    embeddedCliPrint(cli, "[ERROR] temp_profile_set args");
}