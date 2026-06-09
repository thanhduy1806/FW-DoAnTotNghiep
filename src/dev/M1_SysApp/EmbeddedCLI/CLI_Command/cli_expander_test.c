#include "cli_expander_test.h"
#include "stdio.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "stdlib.h"
#include "string.h"
#include "M2_BSP/BSP_Solenoid/bsp_solenoid.h"
#include "M3_Driver/devices/tca6416/tca6416.h"

extern tca6416a_t expander;
/*************************************************
 *             Command List Function             *
 *************************************************/
void CMD_Expander_Write (EmbeddedCli *cli, char *args, void *context)
{
    const char *ValStr = embeddedCliGetToken(args, 1);
    char buf[128];
    
    if(ValStr == NULL)
    {
        embeddedCliPrint(cli, "Usage: expander_write <val>");
        return;
    }
    
    uint16_t value = (uint16_t)strtoul(ValStr, NULL, 0);
    
    tca6416a_write_outputs(&expander,value);
}

void CMD_Expander_Read (EmbeddedCli *cli, char *args, void *context)
{
    uint16_t value = 0;
    char buf[128];
    int rc;
    
    rc = tca6416a_read_inputs(&expander,&value);
    
    snprintf(buf, sizeof(buf), "0x%02X", value);
    embeddedCliPrint(cli, buf);
}