#include "cli_command.h"
#include "M2_BSP/BSP_Board/bsp_core.h"
#include "M2_BSP/BSP_Flow_Sen/bsp_flow_sen.h"
#include "stdio.h"

void CMD_Flow_Sen_ReadAll(EmbeddedCli *cli, char *args, void *context)
{
    (void)args;
    (void)context;

    embeddedCliPrint(cli, "=== FLOW SENSOR READ ALL ===");

    int ret = bsp_flow_sen_read_all();

    if (ret == 0)
    {
        embeddedCliPrint(cli, "All sensors read failed!\r\n");
        return;
    }

    char buf[128];

    for (uint8_t i = 0; i < 4; i++)
    {
        if (s_flow_sen_data[i].read_oke)
        {
            snprintf(buf, sizeof(buf),
                     "[FLOW%d] OK | Flow: %.2f ul/min | Temp: %.2f C | Flags: 0x%04X",
                     i + 1,
                     s_flow_sen_data[i].slf3s_dat.flow,
                     s_flow_sen_data[i].slf3s_dat.temp,
                     s_flow_sen_data[i].slf3s_dat.flags);
        }
        else
        {
            snprintf(buf, sizeof(buf),
                     "[FLOW%d] ERROR",
                     i + 1);
        }

        embeddedCliPrint(cli, buf);
    }

    embeddedCliPrint(cli, "");
}