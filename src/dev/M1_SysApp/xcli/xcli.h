#ifndef XCLI_H
#define XCLI_H
/*
 * xcli.h   xCLI Remote CLI over xTP
 *
 * =========================================================================
 * =========================================================================
 *
 *  1. Single 16-bit xTP ID (XCLI_XTP_ID = 0x0100) for all xCLI traffi
 *
 *  2. ARQ integration: if XTP_USE_ARQ=1, the REQUEST frame is ACKed by
 *     xTP transport automatically.  The RESPONSE frame(s) are sent via
 *     plain xTP_Send (no ARQ)
 *
 *  3. Built-in multi-fragment response: if the serialised fields exceed
 *     XCLI_FRAG_PAYLOAD_MAX bytes, the server splits the response into
 *     multiple xTP frames automatically.  The host reassembles by fragment
 *     index.
 *
 *  4. Response header is 6 bytes:
 *       [DIR][CMD_ID][SEQ][STATUS][TOTAL_FRAGS][FRAG_IDX]
 *
 * =========================================================================
 * SERVER USAGE (MCU)
 * =========================================================================
 * @code
 *   static xTP_Instance_t  g_xtp;
 *   static xTP_ARQ_t       g_arq;   // if ARQ enabled
 *   static xCLI_Instance_t g_xcli;
 *
 *   // Init
 *   g_xtp.port = xTP_GetDefaultPort();
 *   xTP_Init(&g_xtp, 0);
 *   xTP_ARQ_Init(&g_xtp, &g_arq);  // if ARQ enabled
 *   xCLI_Init(&g_xcli, &g_xtp);
 *   xCLI_RegisterBuiltins(&g_xcli);
 *
 *   // Register custom command
 *   xCLI_RegisterCommand(&g_xcli, 0x20, "adc", "Read ADC", adc_handler);
 *
 *   // Main loop
 *   while (1) {
 *       xTP_Process(&g_xtp);        // dispatches to xCLI_Process automatically
 *       xTP_ARQ_Poll(&g_xtp);       // if ARQ enabled
 *   }
 * @endcode
 *
 * =========================================================================
 * CUSTOM COMMAND HANDLER
 * =========================================================================
 * @code
 *   static void adc_handler(xCLI_ReqCtx_t *ctx, const char *args)
 *   {
 *       uint8_t ch = (uint8_t)xcli_token_u32(args, 1U);
 *       if (ch > 7U) { ctx->status = XCLI_STATUS_BAD_ARG; return; }
 *       xCLI_Ser_PutU8 (&ctx->ser, "ch",  ch);
 *       xCLI_Ser_PutU16(&ctx->ser, "raw", adc_read(ch));
 *   }
 * @endcode
 *                                                  C.H
 */

#include <stdint.h>
#include "xcli_config.h"
#include "xcli_commands.h"
#include "xcli_serial.h"
#include "M1_SysApp/xtp/xtp.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Src/embedded_cli.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Return codes
 * =========================================================================*/
typedef enum {
    XCLI_OK           = 0x00,
    XCLI_ERR          = 0x01,
    XCLI_ERR_FULL     = 0x02,
    XCLI_ERR_NOT_INIT = 0x03,
    XCLI_ERR_FRAME    = 0x04
} xCLI_Return_t;

/* =========================================================================
 * Forward declarations
 * =========================================================================*/
typedef struct xCLI_Instance_s xCLI_Instance_t;
typedef struct xCLI_ReqCtx_s   xCLI_ReqCtx_t;

/* =========================================================================
 * Request context - passed to every handler
 * =========================================================================*/
struct xCLI_ReqCtx_s {
    xCLI_Instance_t *xcli;
    uint8_t          cmd_id;
    uint8_t          seq;
    uint8_t          resp_buf[XCLI_MAX_RESP_TOTAL]; /**< Total fields buffer. */
    xCLI_Ser_t       ser;
    uint8_t          handled;
    uint8_t          status;
};

/* Handler signature */
typedef void (*xCLI_Handler_t)(xCLI_ReqCtx_t *ctx, const char *args);

/* =========================================================================
 * Command registry entry
 * =========================================================================*/
typedef struct {
    uint8_t        cmd_id;
    char           name[XCLI_MAX_CMD_NAME];
    char           help[XCLI_MAX_CMD_HELP];
    xCLI_Handler_t handler;
} xCLI_CmdEntry_t;

/* =========================================================================
 * Instance
 * =========================================================================*/
struct xCLI_Instance_s {
    xTP_Instance_t  *xtp;
    EmbeddedCli     *ecli;
    CLI_UINT         ecli_buf[BYTES_TO_CLI_UINTS(XCLI_ECLI_BUF_SIZE)];
    xCLI_CmdEntry_t  cmds[XCLI_MAX_COMMANDS];
    uint8_t          cmd_count;
    xCLI_ReqCtx_t    req_ctx;
    uint8_t          initialized;
};

/* =========================================================================
 * Public API
 * =========================================================================*/

/**
 * @brief Initialise xCLI and register its xTP command handler.
 *
 * After this call, frames arriving with xTP ID == XCLI_XTP_ID are
 * automatically dispatched to xCLI_Process via xTP's command table.
 * The caller must still call xTP_Process() (and xTP_ARQ_Poll if ARQ).
 */
xCLI_Return_t xCLI_Init(xCLI_Instance_t *xcli, xTP_Instance_t *xtp);

/**
 * @brief Register a command handler.
 * Registering the same CMD_ID twice replaces the earlier entry.
 */
xCLI_Return_t xCLI_RegisterCommand(xCLI_Instance_t *xcli,
                                    uint8_t          cmd_id,
                                    const char      *name,
                                    const char      *help,
                                    xCLI_Handler_t   handler);

/**
 * @brief Process an incoming xCLI frame.
 * Called automatically via xTP command table - do NOT call manually.
 */
xCLI_Return_t xCLI_Process(xCLI_Instance_t *xcli,
                             const uint8_t   *payload,
                             uint16_t         len);

/**
 * @brief Build and send an xCLI REQUEST frame (host/MCU-to-MCU use).
 */
xCLI_Return_t xCLI_SendRequest(xCLI_Instance_t *xcli,
                                uint8_t          cmd_id,
                                uint8_t          seq,
                                const char      *cmd_str);

/** Register all built-in handlers. Call once after xCLI_Init(). */
#if XCLI_USE_BUILTIN_COMMANDS
void xCLI_RegisterBuiltins(xCLI_Instance_t *xcli);
#endif

/** Parse decimal/hex token at position pos (1-based) from tokenised args. */
uint32_t xcli_token_u32(const char *tokenized_args, uint16_t pos);

/* =========================================================================
 * Internal xTP command-table callback (do not call directly)
 * =========================================================================*/
void xCLI_XTP_Handler(xTP_Instance_t *inst,
                       const uint8_t  *payload,
                       uint16_t        len);

#ifdef __cplusplus
}
#endif
#endif /* XCLI_H */
