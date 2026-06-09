#include "db_temperature.h"
#include "stdint.h"

int16_t db_temperature[END_OF_DB_TEMP_TABLE];

/**
 * @brief Reads temperature from the database and converts it to float.
 */
float db_temperature_read(e_db_temperature_t index) {
    if (index < END_OF_DB_TEMP_TABLE) {
        // Convert from int16 (e.g., 2550) back to float (e.g., 25.5)
        // Adjust the division factor based on your scaling (1.0f if no scaling)
        return (float)db_temperature[index] / 100.0f;
    }
    return 0.0f;
}

/**
 * @brief Converts float temperature to int16 and writes to the database.
 */
void db_temperature_write(e_db_temperature_t index, float temp) {
    if (index < END_OF_DB_TEMP_TABLE) {
        // Convert from float (e.g., 25.5) to int16 (e.g., 2550)
        // Multiply by 100.0f to keep 2 decimal places in an integer
        db_temperature[index] = (int16_t)(temp * 100.0f);
    }
}