/* 
 * File:   cli_temperature.h
 * Author: HTSANG
 *
 * Created on April 29, 2026, 11:11 PM
 */

#ifndef CLI_TEMPERATURE_H
#define	CLI_TEMPERATURE_H

#include "M1_SysApp/EmbeddedCLI/CLI_Src/embedded_cli.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
//=======================================HUYNH THANH SANG============================================//
///////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {
    GET_CONFIRM = 0,
    GET_PROFILE_INDEX,
    GET_MAIN_NTC,
    GET_SEC_NTC,
    GET_TEC_MASK,
    GET_HEATER_MASK,
    GET_SETPOINT,
    GET_MAIN_SEC_DELTA,
    GET_STEP_COUNT,
    GET_STEP,
    // ...
    GET_SAVE,
    MAX_STEP_GETTING
} e_getting_state_t;

extern e_getting_state_t input_step;

void TempProfile_InputHandler(EmbeddedCli *cli, char *input, uint8_t step);

uint8_t TempProfile_CheckRemainStep(void);

void CMD_DB_TempProfile_display(EmbeddedCli *cli, char *args, void *context);
void CMD_DB_TempProfile_validation(EmbeddedCli *cli, char *args, void *context);

///////////////////////////////////////////////////////////////////////////////////////////////////////
//=======================================HUYNH THANH SANG============================================//
///////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	/* CLI_TEMPERATURE_H */

