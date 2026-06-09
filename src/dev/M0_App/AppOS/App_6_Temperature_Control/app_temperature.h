/*
 * File:   app_temperature.h
 * Author: HTSANG
 *
 * Created on April 16, 2026, 4:17 PM
 */

#ifndef APP_TEMPERATURE_H
#define APP_TEMPERATURE_H

#include "os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef enum {
    TEMP_CMD_OFF = 0,
    TEMP_CMD_EVENT_START,
    TEMP_CMD_MODE_MANUAL,
    TEMP_CMD_MODE_AUTO
} e_temp_cmd_t;

extern bool temp_pid_log_ena;
extern QueueHandle_t temperature_command_queue[8];
extern const osThreadAttr_t temp_attr;

void temperature_init(void);
void App_TemperatureTask(void *param);

#endif /* APP_TEMPERATURE_H */
