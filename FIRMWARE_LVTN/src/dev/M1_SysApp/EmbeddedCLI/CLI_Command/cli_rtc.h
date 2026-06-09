#include "dev/M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"

#include "../../../M2_BSP/BSP_RTC/bsp_rtc.h"
#include "samv71q21b.h"

void CMD_RTC_Read(EmbeddedCli *cli, char *args, void *context);
void CMD_RTC_Set_Date(EmbeddedCli *cli, char *args, void *context);
void CMD_RTC_Set_Time(EmbeddedCli *cli, char *args, void *context);
void CMD_RTC_Ping(EmbeddedCli *cli, char *args, void *context);
void CMD_RTC_Test(EmbeddedCli *cli, char *args, void *context);