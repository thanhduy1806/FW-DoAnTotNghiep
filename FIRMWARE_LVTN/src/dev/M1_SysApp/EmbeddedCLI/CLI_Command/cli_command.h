/*
 * cli_command.h
 *
 *  Created on: Feb 27, 2025
 *      Author:
 */

#ifndef M2_SYSTEM_CLI_TERMINAL_CLI_COMMAND_CLI_COMMAND_H_
#define M2_SYSTEM_CLI_TERMINAL_CLI_COMMAND_CLI_COMMAND_H_

#include "../CLI_Src/embedded_cli.h"

const CliCommandBinding *getCliStaticBindings(void);
uint16_t getCliStaticBindingCount(void);

void CMD_Photo_Turn_On_Channel(EmbeddedCli *cli, char *args, void *context);

void CMD_FRAM_WriteRead(EmbeddedCli *cli, char *args, void *context);

void CMD_PSRAM_Switch_MCU(EmbeddedCli *cli, char *args, void *context);
void CMD_PSRAM_Switch_PMU(EmbeddedCli *cli, char *args, void *context);
void CMD_PSRAM_Read_ID(EmbeddedCli *cli, char *args, void *context);
void CMD_PSRAM_Write_Byte(EmbeddedCli *cli, char *args, void *context);
void CMD_PSRAM_Read_Byte(EmbeddedCli *cli, char *args, void *context);

void CMD_Flow_Sen_ReadAll(EmbeddedCli *cli, char *args, void *context);

void CMD_NTC_Read(EmbeddedCli *cli, char *args, void *context);
void CMD_NTC_Read_All(EmbeddedCli *cli, char *args, void *context);
void CMD_Check_NTC(EmbeddedCli *cli, char *args, void *context);

void CMD_Temp_profile_set(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_auto_ena(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_auto_start(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_manu(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_log_toggle(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_PID_set(EmbeddedCli *cli, char *args, void *context);
void CMD_Temp_PID_get(EmbeddedCli *cli, char *args, void *context);

#endif /* M2_SYSTEM_CLI_TERMINAL_CLI_COMMAND_CLI_COMMAND_H_ */
