#include "dev/M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"

#include "dev/M2_BSP/BSP_Solenoid_Dual/bsp_sol_dual.h"
#include "samv71q21b.h"

/*************************************************
 *                Command Define                 *
 *************************************************/
void CMD_SOL_Dual_Forward (EmbeddedCli *cli, char *args, void *context);
void CMD_SOL_Dual_Reverse(EmbeddedCli *cli, char *args, void *context);