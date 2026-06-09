/************************************************
 *  @file     : app_xtp.c
 *  @date     : April 2026
 *  @author   : CAO HIEU
 ************************************************/
#include "M1_SysApp/xtp/xtp.h"
#include "definitions.h"
#include "M2_BSP/UART/uart_irq.h"
#include "M1_SysApp/xtp/xtp.h"
#include "M1_SysApp/xtp/xtp_arq.h"
#include "M1_SysApp/xcli/xcli.h"
#include "M1_SysApp/xcli/xcli_commands.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"
#include "os.h"

/* =========================================================================
 * Private instances
 * =========================================================================*/
static xTP_Instance_t  s_xtp;
static xCLI_Instance_t s_xcli;

#if XTP_USE_ARQ
static xTP_ARQ_t s_arq;
#endif

#define BRIDGE_CHUNK_SIZE  241U

/* Total capture buffer: 8 chunks x 241 = 1928 bytes
 * (XCLI_MAX_RESP_TOTAL=2048 overhead)                       */
#define BRIDGE_NUM_CHUNKS  8U
#define BRIDGE_BUF_SIZE    (BRIDGE_CHUNK_SIZE * BRIDGE_NUM_CHUNKS)  /* 1928 bytes */

static char s_bridge_buf[BRIDGE_BUF_SIZE + 1U];  /* +1 for NUL terminator */

static void xcli_cli_bridge(xCLI_ReqCtx_t *ctx, const char *args)
{
    char     cmd_str[XCLI_MAX_CMD_STR];
    uint16_t pos = 0;
    uint16_t tok = 1;
    const char *t;
    size_t   out_len;
    uint8_t  n_chunks;
    uint8_t  truncated;
    char     key[5];    /* "c_0" .. "c_99" */
    uint16_t i;

    /* ---- Reconstruct command string from tokenized args ---- */
    while ((t = embeddedCliGetToken(args, tok)) != NULL
           && pos < sizeof(cmd_str) - 2U) {
        if (tok > 1U) { cmd_str[pos++] = ' '; }
        uint16_t tlen = (uint16_t)strlen(t);
        if (pos + tlen >= sizeof(cmd_str) - 1U) { break; }
        memcpy(&cmd_str[pos], t, (size_t)tlen);
        pos += tlen;
        tok++;
    }
    cmd_str[pos] = '\0';

    if (pos == 0U) {
        ctx->status = XCLI_STATUS_BAD_ARG;
        xCLI_Ser_PutStr(&ctx->ser, "err", "empty command");
        return;
    }

    /* ---- Execute via >> RGOSH CLI ---- */
    EmbeddedCli *rgosh = get_RGOSH_CliPointer();
    if (rgosh == NULL) {
        ctx->status = XCLI_STATUS_ERR;
        xCLI_Ser_PutStr(&ctx->ser, "err", "rgosh not ready");
        return;
    }

    memset(s_bridge_buf, 0, sizeof(s_bridge_buf));
    Cli_RGOSH_CaptureStart(s_bridge_buf, BRIDGE_BUF_SIZE); /* cap at 1928 */
    for (i = 0U; cmd_str[i] != '\0'; i++) {
        embeddedCliReceiveChar(rgosh, cmd_str[i]);
    }
    embeddedCliReceiveChar(rgosh, '\n');
    embeddedCliProcess(rgosh);
    Cli_RGOSH_CaptureStop(&out_len);

    truncated = (out_len >= BRIDGE_BUF_SIZE) ? 1U : 0U;

    /* ---- Serialize as chunks ---- */
    /* Number of chunks needed */
    n_chunks = (uint8_t)((out_len + BRIDGE_CHUNK_SIZE - 1U) / BRIDGE_CHUNK_SIZE);
    if (n_chunks == 0U && out_len == 0U) { n_chunks = 0U; }
    if (n_chunks > BRIDGE_NUM_CHUNKS)    { n_chunks = (uint8_t)BRIDGE_NUM_CHUNKS; }

    xCLI_Ser_PutU8(&ctx->ser, "n", n_chunks);

    for (i = 0U; i < (uint16_t)n_chunks; i++) {
        uint16_t offset     = (uint16_t)(i * BRIDGE_CHUNK_SIZE);
        uint16_t chunk_len  = (uint16_t)out_len - offset;
        char     chunk_buf[BRIDGE_CHUNK_SIZE + 1U];

        if (chunk_len > BRIDGE_CHUNK_SIZE) { chunk_len = BRIDGE_CHUNK_SIZE; }

        /* Build key "c_0" .. "c_7" */
        key[0] = 'c'; key[1] = '_';
        if (i < 10U) {
            key[2] = (char)('0' + i); key[3] = '\0';
        } else {
            key[2] = (char)('0' + i / 10U);
            key[3] = (char)('0' + i % 10U);
            key[4] = '\0';
        }

        memcpy(chunk_buf, &s_bridge_buf[offset], (size_t)chunk_len);
        chunk_buf[chunk_len] = '\0';

        xCLI_Ser_PutStr(&ctx->ser, key, chunk_buf);

        if (xCLI_Ser_Error(&ctx->ser)) { break; } /* resp_buf full */
    }

    xCLI_Ser_PutBool(&ctx->ser, "trunc", truncated);
}

/* =========================================================================
 * local_help (CMD_ID = 0x31)
 * =========================================================================*/
#define LOCAL_HELP_PAGE_SIZE  20U

static void xcli_local_help(xCLI_ReqCtx_t *ctx, const char *args)
{
    const CliCommandBinding *bindings = getCliStaticBindings();
    uint16_t total  = (uint16_t)getCliStaticBindingCount();
    uint16_t offset = (uint16_t)xcli_token_u32(args, 1U);
    uint16_t end, i;
    uint8_t  count;
    char     key[5]; /* "n_0".."c_19" */

    if (offset >= total) { offset = 0U; }
    end   = offset + LOCAL_HELP_PAGE_SIZE;
    if (end > total) { end = total; }
    count = (uint8_t)(end - offset);

    xCLI_Ser_PutU16(&ctx->ser, "total",  total);
    xCLI_Ser_PutU16(&ctx->ser, "offset", offset);
    xCLI_Ser_PutU8 (&ctx->ser, "count",  count);

    for (i = offset; i < end; i++) {
        uint8_t idx = (uint8_t)(i - offset);

        /* "n_0".."n_19" */
        key[0] = 'n'; key[1] = '_';
        if (idx < 10U) { key[2] = (char)('0' + idx); key[3] = '\0'; }
        else           { key[2] = (char)('0' + idx / 10U); key[3] = (char)('0' + idx % 10U); key[4] = '\0'; }
        xCLI_Ser_PutStr(&ctx->ser, key, bindings[i].name ? bindings[i].name : "");

        /* "c_0".."c_19" */
        key[0] = 'c';
        xCLI_Ser_PutStr(&ctx->ser, key, bindings[i].category ? bindings[i].category : "");

        if (xCLI_Ser_Error(&ctx->ser)) { break; }
    }
}

/* =========================================================================
 * Task
 * =========================================================================*/
const osThreadAttr_t xtp_attr = {
    .name       = "XTP",
    .stack_size = configMINIMAL_STACK_SIZE * 5,
    .priority   = configMAX_PRIORITIES - 3,
};

void App_XTPTask(void *param)
{
    (void)param;

    s_xtp.port = xTP_GetDefaultPort();
    xTP_Init(&s_xtp, 0x01U);

#if XTP_USE_ARQ
    xTP_ARQ_Init(&s_xtp, &s_arq);
#endif

    if (xCLI_Init(&s_xcli, &s_xtp) != XCLI_OK) {
        xTP_Log("xCLI: init FAILED");
        vTaskDelete(NULL);
        return;
    }

    xCLI_RegisterBuiltins(&s_xcli);
    xCLI_RegisterCommand(&s_xcli, XCLI_CMD_CLI,
                         "cli", "Tunnel local CLI cmd: cli <cmd> [args...]",
                         xcli_cli_bridge);
    xCLI_RegisterCommand(&s_xcli, XCLI_CMD_LOCAL_HELP,
                         "local_help", "List MCU CLI commands: local_help [offset]",
                         xcli_local_help);

    xTP_Log("App_XTPTask ready. CMD=0x%04X RESP=0x%04X",
            (unsigned)XCLI_XTP_CMD_ID, (unsigned)XCLI_XTP_RESP_ID);

    while (1) {
        xTP_Process(&s_xtp);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}