#include "cli_command.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "errno.h"

#include "M2_BSP/BSP_Board/bsp_core.h"
#include "M2_BSP/BSP_PSRAM/bsp_psram.h"

#define PAGE_SIZE               1024U
#define PSRAM_CLI_MAX_BYTES     256U
#define PSRAM_DUMP_BYTES_LINE   16U

static bool cli_parse_u32(const char *str, uint32_t *value) {
    char *endptr = NULL;
    unsigned long tmp;

    if (str == NULL || value == NULL) {
        return false;
    }

    errno = 0;
    tmp = strtoul(str, &endptr, 0);

    if ((errno != 0) || (endptr == str) || (*endptr != '\0')) {
        return false;
    }

    *value = (uint32_t) tmp;
    return true;
}

static void psram_cli_print_hex_dump(EmbeddedCli *cli, uint32_t base_addr, const uint8_t *data, uint32_t length) {
    char line_buf[256];
    char byte_buf[8];
    uint32_t pos;

    if (cli == NULL || data == NULL || length == 0U) {
        return;
    }
    for (uint32_t i = 0; i < length; i += 16U) {
        pos = (uint32_t) snprintf(line_buf, sizeof (line_buf), "0x%08lX: ", (unsigned long) (base_addr + i));
        for (uint32_t j = 0; (j < 16U) && ((i + j) < length); j++) {
            snprintf(byte_buf, sizeof (byte_buf), "0x%02X ", data[i + j]);
            pos += (uint32_t) snprintf(&line_buf[pos], sizeof (line_buf) - pos, "%s", byte_buf);
        }
        embeddedCliPrint(cli, line_buf);
    }
}

void CMD_PSRAM_Switch_MCU(EmbeddedCli *cli, char *args, void *context) {
    (void) cli;
    (void) args;
    (void) context;

    do_reset(&switch_spi_psram);
}

void CMD_PSRAM_Switch_PMU(EmbeddedCli *cli, char *args, void *context) {
    (void) cli;
    (void) args;
    (void) context;

    do_set(&switch_spi_psram);
}

void CMD_PSRAM_Read_ID(EmbeddedCli *cli, char *args, void *context) {
    (void) args;
    (void) context;

    char buf[128];

    if (bsp_psram_read_id(&g_psram) != ERROR_OK) {
        embeddedCliPrint(cli, "Error: PSRAM read ID failed");
        return;
    }

    snprintf(buf, sizeof (buf),
            "PSRAM ID: %02X %02X %02X %02X %02X %02X %02X %02X",
            g_psram.id[0], g_psram.id[1], g_psram.id[2], g_psram.id[3],
            g_psram.id[4], g_psram.id[5], g_psram.id[6], g_psram.id[7]);
    embeddedCliPrint(cli, buf);

    if ((g_psram.id[0] != 0x9DU) || (g_psram.id[1] != 0x5DU)) {
        embeddedCliPrint(cli, "Wrong ID");
        return;
    }

    embeddedCliPrint(cli, "PASS");
}

void CMD_PSRAM_Write_Byte(EmbeddedCli *cli, char *args, void *context) {
    (void) context;

    char buf[128];
    uint32_t psram_addr;
    uint8_t tx_buf[PSRAM_CLI_MAX_BYTES];
    uint32_t length = 0U;

    const char *addr_str = embeddedCliGetToken(args, 1);

    if (addr_str == NULL) {
        embeddedCliPrint(cli, "Usage: psram_write <address> <byte1> [byte2] ...");
        embeddedCliPrint(cli, "Example: psram_write 0x100 0x11 0x22 0x33");
        return;
    }

    if (!cli_parse_u32(addr_str, &psram_addr)) {
        embeddedCliPrint(cli, "Error: invalid address");
        return;
    }

    for (uint32_t token_idx = 2U; token_idx < (PSRAM_CLI_MAX_BYTES + 2U); token_idx++) {
        const char *token = embeddedCliGetToken(args, token_idx);
        uint32_t value;

        if (token == NULL) {
            break;
        }

        if (!cli_parse_u32(token, &value) || value > 0xFFU) {
            snprintf(buf, sizeof (buf),
                    "Error: invalid byte '%s' (range: 0x00 - 0xFF)",
                    token);
            embeddedCliPrint(cli, buf);
            return;
        }

        tx_buf[length++] = (uint8_t) value;
    }

    if (length == 0U) {
        embeddedCliPrint(cli, "Error: no data bytes provided");
        return;
    }

    if (bsp_psram_write(&g_psram, psram_addr, tx_buf, length) != ERROR_OK) {
        embeddedCliPrint(cli, "Error: PSRAM write failed");
        return;
    }

    snprintf(buf, sizeof (buf),
            "PSRAM Write @0x%08lX, %lu bytes OK",
            (unsigned long) psram_addr,
            (unsigned long) length);
    embeddedCliPrint(cli, buf);

    embeddedCliPrint(cli, "Data:");
    psram_cli_print_hex_dump(cli, psram_addr, tx_buf, length);

    embeddedCliPrint(cli, "Done");
}

void CMD_PSRAM_Read_Byte(EmbeddedCli *cli, char *args, void *context) {
    (void) context;

    char buf[256];
    char line_buf[256];

    uint32_t psram_addr;
    uint32_t length;
    uint32_t aligned_addr;
    uint32_t offset;
    uint32_t read_len;
    uint32_t overflow_page;

    uint8_t rx_buf[PSRAM_CLI_MAX_BYTES];

    const char *addr_str = embeddedCliGetToken(args, 1);
    const char *len_str  = embeddedCliGetToken(args, 2);

    if ((addr_str == NULL) || (len_str == NULL)) {
        embeddedCliPrint(cli, "Usage: psram_read <address> <length>");
        embeddedCliPrint(cli, "Example: psram_read 0x100 16");
        return;
    }

    if (!cli_parse_u32(addr_str, &psram_addr)) {
        embeddedCliPrint(cli, "Error: invalid address");
        return;
    }

    if (!cli_parse_u32(len_str, &length)) {
        embeddedCliPrint(cli, "Error: invalid length");
        return;
    }

    if (length == 0U) {
        embeddedCliPrint(cli, "Error: length must be > 0");
        return;
    }

    offset = psram_addr % 16U;
    aligned_addr = psram_addr - offset;
    read_len = length + offset;
    overflow_page = ((aligned_addr + read_len) / PAGE_SIZE) - (aligned_addr / PAGE_SIZE);
            
    if (read_len > PSRAM_CLI_MAX_BYTES) {
        snprintf(buf, sizeof(buf), "Error: max read is %u bytes after alignment", PSRAM_CLI_MAX_BYTES);
        embeddedCliPrint(cli, buf);
        return;
    }

    if (offset != 0U) {
        embeddedCliPrint(cli, "Warning: address is not aligned to 1024 bytes");
    }

    if (bsp_psram_read(&g_psram, aligned_addr, rx_buf, read_len) != ERROR_OK) {
        embeddedCliPrint(cli, "Error: PSRAM read failed");
        return;
    }

    snprintf(buf, sizeof(buf), "PSRAM Read @0x%08lX, %lu bytes:", (unsigned long)aligned_addr, (unsigned long)read_len);
    embeddedCliPrint(cli, buf);

    for (uint32_t i = 0; i < read_len; i += 16U) {
        uint32_t pos;
        if (overflow_page) {
            if (((aligned_addr + i) % PAGE_SIZE) == 0) {
                embeddedCliPrint(cli, "--- END PAGE ---");
                embeddedCliPrint(cli, "Warning: read overflow page boundary, data may wrap around to start of page");
                embeddedCliPrint(cli, "--- START PAGE ---");
                aligned_addr -= PAGE_SIZE;
            }
        }
        pos = (uint32_t)snprintf(line_buf, sizeof(line_buf), "0x%08lX: ", (unsigned long)(aligned_addr + i));
        for (uint32_t j = 0U; (j < 16U) && ((i + j) < read_len); j++) {
            pos += (uint32_t)snprintf(&line_buf[pos], sizeof(line_buf) - pos, "0x%02X ", rx_buf[i + j]);
            if (pos >= (sizeof(line_buf) - 8U)) {
                break;
            }
        }
        embeddedCliPrint(cli, line_buf);
    }
}