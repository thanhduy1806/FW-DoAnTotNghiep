#include "cli_sol_dual.h"

void CMD_SOL_Dual_Forward (EmbeddedCli *cli, char *args, void *context)
{
    const char *idexStr = embeddedCliGetToken(args, 1);
    char buf[128];
    
    if (idexStr == NULL) {
        embeddedCliPrint(cli, "Usage: sol_dual_forward <index>");
        return;
    }

    uint8_t index = (uint8_t) strtoul(idexStr, NULL, 0);

    if ((index < 1) || (index > 4)) {
        embeddedCliPrint(cli, "Index 1 to 4");
        return;
    }
    
    bsp_sol_dual_forward(&sol_dual[index-1]);
}

void CMD_SOL_Dual_Reverse (EmbeddedCli *cli, char *args, void *context)
{
    const char *idexStr = embeddedCliGetToken(args, 1);
    char buf[128];
    
    if (idexStr == NULL) {
        embeddedCliPrint(cli, "Usage: sol_dual_reverse <index>");
        return;
    }

    uint8_t index = (uint8_t) strtoul(idexStr, NULL, 0);

    if (index < 1 || index > 4) {
        embeddedCliPrint(cli, "Index 1 to 4");
        return;
    }
    
    bsp_sol_dual_reverse(&sol_dual[index-1]);
}