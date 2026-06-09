/* 
 * File:   db_temperature.h
 * Author: HTSANG
 *
 * Created on April 10, 2026, 2:43 PM
 */

#ifndef E_DB_TEMPERATURE_H
#define	E_DB_TEMPERATURE_H

#include "stdint.h"

typedef enum {
    NTC_1 = 0,
    NTC_2,
    NTC_3,
    NTC_4,
    NTC_5,
    NTC_6,
    NTC_7,
    NTC_8,
    BOARD_TEMP,
    END_OF_DB_TEMP_TABLE,
} e_db_temperature_t;


float db_temperature_read(e_db_temperature_t index);
void db_temperature_write(e_db_temperature_t index, float temp);

#endif	/* E_DB_TEMPERATURE_H */

