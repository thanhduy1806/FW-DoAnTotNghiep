#include "cli_command.h"
#include "stdio.h"
#include "stdlib.h"

#include "M2_BSP/BSP_Photo/bsp_photo.h"


void CMD_Photo_Turn_On_Channel(EmbeddedCli *cli, char *args, void *context) {
    const char *token = embeddedCliGetToken(args, 1);

    if (token == NULL) {
        embeddedCliPrint(cli, "Error: missing channel");
        return;
    }

    uint8_t channel = (uint8_t) strtoul(token, NULL, 0);

    if (channel >= PHOTO_CHANNEL_MAX) {
        embeddedCliPrint(cli, "Error: invalid channel (rcm: 1-24)");
        return;
    }

    bsp_photo_sw_on(channel);

    char buf[64];
    sprintf(buf, "Photo channel %d turned ON", channel);
    embeddedCliPrint(cli, buf);
}

        
