#include "dev/M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"
#include "M2_BSP/BSP_NTC/bsp_ntc.h"
#include "samv71q21b.h"

/*************************************************
 *                Command Define                 *
 *************************************************/
void CMD_NTC_Read_All (EmbeddedCli *cli, char *args, void *context);
void CMD_NTC_Read(EmbeddedCli *cli, char *args, void *context);
void CMD_Check_NTC(EmbeddedCli *cli, char *args, void *context);