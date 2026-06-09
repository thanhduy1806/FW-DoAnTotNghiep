#include "dev/M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"

#include "../../../M2_BSP/BSP_Pump/bsp_pump.h"
#include "samv71q21b.h"

void CMD_PUMP_Enable(EmbeddedCli *cli, char *args, void *context);
void CMD_PUMP_Disable(EmbeddedCli *cli, char *args, void *context);
void CMD_PUMP_Set_Volt(EmbeddedCli *cli, char *args, void *context);
void CMD_PUMP_Set_Freq(EmbeddedCli *cli, char *args, void *context);