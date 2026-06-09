#include "cli_laser.h"
#include "../../../M2_BSP/BSP_Laser/bsp_laser.h"

void CMD_Laser_Int_Set_DAC(EmbeddedCli *cli, char *args, void *context) {
    const char *valStr = embeddedCliGetToken(args, 1);
    char *endptr;
    char buf[120];

    if (valStr == NULL) {
        embeddedCliPrint(cli, "Usage: laser_dac <value>");
        return;
    }

    uint32_t val = strtoul(valStr, &endptr, 0);
    if (*endptr != '\0' || val > 255) {
        embeddedCliPrint(cli, "Invalid DAC value (0-255)");
        return;
    }

    bsp_laser_int_set_dac((uint8_t) val);

    snprintf(buf, sizeof (buf),
            "Laser DAC set to: %lu", val);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_Laser_Int_Turn_On_Channel(EmbeddedCli *cli, char *args, void *context) {
    const char *chStr = embeddedCliGetToken(args, 1);
    char *endptr;
    char buf[120];

    if (chStr == NULL) {
        embeddedCliPrint(cli, "Usage: laser_on <channel>");
        return;
    }

    uint32_t ch = strtoul(chStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid channel");
        return;
    }

    bsp_laser_int_sw_on((uint8_t) ch);

    snprintf(buf, sizeof (buf),
            "Laser channel %lu ON", ch);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_Laser_Int_Turn_Off_Channel(EmbeddedCli *cli, char *args, void *context) {
    const char *chStr = embeddedCliGetToken(args, 1);
    char *endptr;
    char buf[120];

    if (chStr == NULL) {
        embeddedCliPrint(cli, "Usage: laser_off <channel>");
        return;
    }

    uint32_t ch = strtoul(chStr, &endptr, 0);
    if (*endptr != '\0') {
        embeddedCliPrint(cli, "Invalid channel");
        return;
    }

    bsp_laser_int_sw_off((uint8_t) ch);

    snprintf(buf, sizeof (buf),
            "Laser channel %lu OFF", ch);

    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_Laser_Int_Turn_Off_All(EmbeddedCli *cli, char *args, void *context) {
    bsp_laser_int_all_sw_off();
}
