#include "cli_command.h"
#include "stdio.h"
#include "stdlib.h"

#include "M0_App/AppOS/App_4_NTC/app_ntc.h"

#include "M2_BSP/BSP_Board/bsp_core.h"
#include "M2_BSP/BSP_NTC/bsp_ntc.h"

/*************************************************
 *             Command List Function             *
 *************************************************/

void CMD_NTC_Read(EmbeddedCli *cli, char *args, void *context) {
    const char *idxStr = embeddedCliGetToken(args, 1);
    char buf[64];

    if (idxStr == NULL) {
        embeddedCliPrint(cli, "Usage: ntc_read <index>");
        return;
    }

    // Convert string argument to index (0-based)
    uint8_t index = (uint8_t) strtoul(idxStr, NULL, 0);

    // Validate index range
    if (index >= END_OF_DB_TEMP_TABLE) {
        embeddedCliPrint(cli, "Error: Index out of range.");
        return;
    }

    if (xSemaphoreTake(ntc_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        float temp_val = db_temperature_read((e_db_temperature_t) index);

        // Handling BOARD_TEMP naming specifically for clarity
        if (index == BOARD_TEMP) {
            snprintf(buf, sizeof (buf), "BOARD_TEMP: %.2f C", temp_val);
        } else {
            snprintf(buf, sizeof (buf), "NTC[%u]: %.2f C", index, temp_val);
        }

        embeddedCliPrint(cli, buf);
        xSemaphoreGive(ntc_mutex);
    } else {
        embeddedCliPrint(cli, "Error: Database busy.");
    }
}

void CMD_NTC_Read_All(EmbeddedCli *cli, char *args, void *context) {
    char msg[64]; // Buffer to hold the formatted string

    embeddedCliPrint(cli, "--- Current Temperature Data ---");

    // Protect data access if the NTC task might be writing simultaneously
    if (xSemaphoreTake(ntc_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {

        for (int i = 0; i < END_OF_DB_TEMP_TABLE - 1; i++) {
            // 1. Read and convert value to float
            float temp_val = db_temperature_read((e_db_temperature_t) i);

            snprintf(msg, sizeof (msg), "NTC[%d]: %.2f C", i, temp_val);

            embeddedCliPrint(cli, msg);
        }

        xSemaphoreGive(ntc_mutex);
    } else {
        embeddedCliPrint(cli, "Error: Could not access temperature database (Busy)");
    }
}

void CMD_Check_NTC(EmbeddedCli *cli, char *args, void *context) {
    bool all_ok = true;
    char msg[64];
    const float LIMIT_MIN = -100.0f;
    const float LIMIT_MAX = 100.0f;

    // Take the mutex to ensure we read stable data
    if (xSemaphoreTake(ntc_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        
        for (int i = 0; i < END_OF_DB_TEMP_TABLE; i++) {
            float current_temp = db_temperature_read((e_db_temperature_t)i);

            // Check if temperature is out of bounds
            if (current_temp < LIMIT_MIN || current_temp > LIMIT_MAX) {
                all_ok = false;
                
                // Identify which sensor failed
                if (i == BOARD_TEMP) {
                    snprintf(msg, sizeof(msg), "FAIL: BOARD_TEMP is %.2f C", current_temp);
                } else {
                    snprintf(msg, sizeof(msg), "FAIL: NTC[%d] is %.2f C", i + 1, current_temp);
                }
                embeddedCliPrint(cli, msg);
            }
        }
        
        xSemaphoreGive(ntc_mutex);

        // If no sensors failed the check
        if (all_ok) {
            embeddedCliPrint(cli, "NTC Status: OK");
        }
    } else {
        embeddedCliPrint(cli, "Error: Could not access database to check NTCs.");
    }
}