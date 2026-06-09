#ifndef CLI_SWITH_TO_ADC_H
#define CLI_SWITH_TO_ADC_H

#include "dev/M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"

/*************************************************
 *                Command Define                 *
 *************************************************/
void CMD_Switch_On_To_ADC(EmbeddedCli *cli, char *args, void *context);
void CMD_Switch_Off_To_ADC(EmbeddedCli *cli, char *args, void *context);
void CMD_Get_All_Switch_Status(EmbeddedCli *cli, char *args, void *context);
void CMD_Get_ADC_Value_Monitor(EmbeddedCli *cli, char *args, void *context);
void CMD_Get_Voltage_Value(EmbeddedCli *cli, char *args, void *context);
void CMD_Get_Current_Value_Power(EmbeddedCli *cli, char *args, void *context);

#endif /* CLI_SWITH_TO_ADC_H */