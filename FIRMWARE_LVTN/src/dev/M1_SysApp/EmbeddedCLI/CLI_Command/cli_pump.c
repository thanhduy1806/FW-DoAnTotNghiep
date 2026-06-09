#include "cli_pump.h"
#include "stdio.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "stdlib.h"
#include "string.h"
#include "os.h"

void CMD_PUMP_Enable(EmbeddedCli *cli, char *args, void *context)
{
    bsp_pump_enable(true);
    
    embeddedCliPrint(cli, "Enable HighDriver PUMP");
}

void CMD_PUMP_Disable(EmbeddedCli *cli, char *args, void *context)
{
    bsp_pump_enable(false);
    
    embeddedCliPrint(cli, "Disable HighDriver PUMP");
}

void CMD_PUMP_Set_Volt(EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    
    const char *VoltStr = embeddedCliGetToken(args, 1);
    
    if (VoltStr == NULL) {
        snprintf(buf, sizeof (buf), "Usage: pump_set_volt <Vpp>");
        embeddedCliPrint(cli, buf);
        return;
    }
    
    uint8_t volt = (uint8_t)strtoul(VoltStr, NULL, 0);
    
    bsp_pump_set_voltage(volt);
    
    snprintf(buf, sizeof (buf), "Volt set:%u Vpp", volt);
    embeddedCliPrint(cli, buf);
}

void CMD_PUMP_Set_Freq(EmbeddedCli *cli, char *args, void *context)
{
    char buf[128];
    
    const char *FreqStr = embeddedCliGetToken(args, 1);
    
    if (FreqStr == NULL) {
        snprintf(buf, sizeof (buf), "Usage: pump_set_freq <Hz>");
        embeddedCliPrint(cli, buf);
        return;
    }
    
    uint16_t freq = (uint16_t)strtoul(FreqStr, NULL, 0);
    
    bsp_pump_set_freq(freq);
    
    snprintf(buf, sizeof (buf), "Freq set:%u Hz", freq);
    embeddedCliPrint(cli, buf);
}