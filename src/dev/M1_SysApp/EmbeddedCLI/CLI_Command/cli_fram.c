#include "cli_command.h"
#include "M2_BSP/BSP_Board/bsp_core.h"
#include "M2_BSP/BSP_FRAM/bsp_fram.h"

void CMD_FRAM_WriteRead(EmbeddedCli *cli, char *args, void *context)
{
    uint32_t addr = 0x00;
    uint8_t tx_buf[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA};
    uint8_t rx_buf[10];
    
    int st = bsp_fram_write(&g_fram, addr, tx_buf, 10);

    if (st != ERROR_OK) {
        embeddedCliPrint(cli, "Write FAIL\r\n");
        return;
    }

    st = bsp_fram_read(&g_fram, addr, rx_buf, 10);

    if (st != ERROR_OK) {
        embeddedCliPrint(cli, "Read FAIL\r\n");
        return;
    }

    for (uint8_t i = 0; i <10; i++)
    {
        if (tx_buf[i] != rx_buf[i])
        {
            embeddedCliPrint(cli, "Wrong data\r\n");
            return;
        }
    }
    embeddedCliPrint(cli, "PASS\r\n");
}