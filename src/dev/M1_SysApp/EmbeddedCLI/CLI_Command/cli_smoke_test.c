#include "cli_smoke_test.h"
#include "stdio.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "stdlib.h"
#include "string.h"

/*************************************************
 *             Command List Function             *
 *************************************************/
void CMD_CLI_Echo (EmbeddedCli *cli, char *args, void *context) {
    char buffer[128];
    const char *EchoStr;
    int pos = 1;
    int offset = 0;

    while ((EchoStr = embeddedCliGetToken(args, pos)) != NULL)
    {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%s%s", pos == 1 ? "" : " ", EchoStr);
        pos++;
    }

    embeddedCliPrint(cli, buffer);
}
