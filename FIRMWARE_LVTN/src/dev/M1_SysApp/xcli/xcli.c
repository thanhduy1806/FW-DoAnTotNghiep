/*
 * xcli.c  -  xCLI core
 *
 * Key points:
 *  - One xTP ID (XCLI_XTP_ID) for both request and response.
 *  - xCLI_Init() registers xCLI_XTP_Handler in xTP command table so
 *    frames are dispatched automatically; caller only needs xTP_Process().
 *  - If XTP_USE_ARQ: request is ACKed by xTP transport
 *    Response frames are sent with plain xTP_Send.
 *  - Large responses are fragmented inside xCLI:
 *      fields_total / XCLI_FRAG_PAYLOAD_MAX fragments, each <= one xTP frame.
 *                                                  C.H
 */

#include "xcli.h"
#include "xcli_commands.h"
#include "xcli_serial.h"
#include "xcli_log.h"

#include <string.h>
#if XTP_USE_ARQ
#include "M1_SysApp/xtp/xtp_arq.h"
#endif
/* =========================================================================
 * Globals - one xCLI instance pointer registered per xTP instance.
 * We store it so the xTP callback can reach it.
 * (Single instance per translation unit - sufficient for bare-metal MCU.)
 * =========================================================================*/
static xCLI_Instance_t *s_xcli_registered = NULL;

/* =========================================================================
 * Internal helpers
 * =========================================================================*/

static void xcli_nop_write_char(EmbeddedCli *cli, char c)
{
    (void)cli; (void)c;
}

static void xcli_on_unknown(EmbeddedCli *cli, CliCommand *command)
{
    (void)cli; (void)command;
}

static uint32_t xcli_parse_u32(const char *s)
{
    uint32_t val = 0U;
    if (s == NULL) { return 0U; }
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        while (*s != '\0') {
            val <<= 4U;
            if      (*s >= '0' && *s <= '9') { val |= (uint32_t)(*s - '0'); }
            else if (*s >= 'a' && *s <= 'f') { val |= (uint32_t)(*s - 'a' + 10); }
            else if (*s >= 'A' && *s <= 'F') { val |= (uint32_t)(*s - 'A' + 10); }
            else                             { break; }
            s++;
        }
    } else {
        while (*s >= '0' && *s <= '9') {
            val = val * 10U + (uint32_t)(*s - '0');
            s++;
        }
    }
    return val;
}

/**
 * @brief Transmit all response fragments for the current request.
 *
 * Fields are pre-serialised in req->resp_buf[0..fields_len-1].
 * We split them into fragments of <= XCLI_FRAG_PAYLOAD_MAX bytes,
 * always splitting at field boundaries.
 *
 * Each fragment is sent as one xTP_Send (no ARQ).
 */
static void xcli_send_response(xCLI_Instance_t *xcli,
                                uint8_t          cmd_id,
                                uint8_t          seq,
                                uint8_t          status,
                                const uint8_t   *fields,
                                uint16_t         fields_len)
{
    /* --- Count fragments --- */
    uint8_t  total_frags;
    static uint16_t frag_sizes[255]; /* static: single-instance, saves 510B stack */
    uint8_t  num_frags = 0U;
    uint16_t pos       = 0U;

    if (fields_len == 0U) {
        /* No fields: single empty fragment. */
        total_frags    = 1U;
        frag_sizes[0]  = 0U;
        num_frags      = 1U;
    } else {
        /*
         * Walk the fields to find split points.
         * A "split point" is between complete fields.
         * If a single field is larger than XCLI_FRAG_PAYLOAD_MAX
         * (unusual), it goes alone in its own fragment.
         */
        uint16_t frag_start = 0U;
        uint16_t frag_used  = 0U;

        while (pos < fields_len) {
            /* Peek at the next field's total wire size. */
            uint16_t field_wire;
            uint8_t  name_len;
            uint16_t data_len;

            if (pos + 1U > fields_len) { break; } /* truncated */
            name_len = fields[pos];
            if (pos + 1U + (uint16_t)name_len + 3U > fields_len) { break; }
            data_len  = (uint16_t)(fields[pos + 1U + name_len + 1U]);
            data_len |= (uint16_t)((uint16_t)(fields[pos + 1U + name_len + 2U]) << 8U);
            field_wire = (uint16_t)(1U + (uint16_t)name_len + 1U + 2U + data_len);

            if (frag_used > 0U &&
                (uint16_t)(frag_used + field_wire) > XCLI_FRAG_PAYLOAD_MAX) {
                /* This field does not fit - close the current fragment. */
                if (num_frags < 255U) {
                    frag_sizes[num_frags++] = (uint16_t)(pos - frag_start);
                }
                frag_start = pos;
                frag_used  = 0U;
            }

            frag_used = (uint16_t)(frag_used + field_wire);
            pos       = (uint16_t)(pos + field_wire);

            /* If frag is now exactly full, close it. */
            if (frag_used >= XCLI_FRAG_PAYLOAD_MAX && num_frags < 255U) {
                frag_sizes[num_frags++] = (uint16_t)(pos - frag_start);
                frag_start = pos;
                frag_used  = 0U;
            }
        }

        /* Last (possibly partial) fragment. */
        if (frag_used > 0U && num_frags < 255U) {
            frag_sizes[num_frags++] = (uint16_t)(pos - frag_start);
        }

        if (num_frags == 0U) {
            /* Edge case: fields_len > 0 but all bytes were truncated. */
            total_frags   = 1U;
            frag_sizes[0] = 0U;
            num_frags     = 1U;
        } else {
            total_frags = (uint8_t)(num_frags & 0xFFU);
        }
    }

    /* --- Send each fragment --- */
    {
        static uint8_t frame[XTP_DATA_MAX_LEN]; /* static: saves 255B stack */
        uint16_t src_pos = 0U;
        uint8_t  fi;

        for (fi = 0U; fi < num_frags; fi++) {
            uint16_t chunk = frag_sizes[fi];
            uint16_t flen;

            frame[0] = XCLI_DIR_RESPONSE;
            frame[1] = cmd_id;
            frame[2] = seq;
            frame[3] = status;
            frame[4] = total_frags;
            frame[5] = fi;
            flen = XCLI_RESP_HDR_LEN;

            if (chunk > 0U && fields != NULL) {
                (void)memcpy(&frame[flen], &fields[src_pos], (size_t)chunk);
                flen    = (uint16_t)(flen + chunk);
                src_pos = (uint16_t)(src_pos + chunk);
            }

            xCLI_Log(">> RESP  cmd=0x%02X seq=%u st=0x%02X frag=%u/%u  %u bytes",
                     (unsigned)cmd_id, (unsigned)seq, (unsigned)status,
                     (unsigned)(fi + 1U), (unsigned)total_frags,
                     (unsigned)flen);

            /* Response frames use plain Send via RESP_ID (no ARQ - avoids
             * PC sending back ACK which would desync MCU ARQ seq counter). */
            (void)xTP_Send(xcli->xtp, XCLI_XTP_RESP_ID, 0xFFU, frame, flen);
        }
    }
}

/* =========================================================================
 * xTP command-table callback - registered by xCLI_Init
 * =========================================================================*/
void xCLI_XTP_Handler(xTP_Instance_t *inst,
                       const uint8_t  *payload,
                       uint16_t        len)
{
    (void)inst;
    if (s_xcli_registered == NULL) { return; }
    (void)xCLI_Process(s_xcli_registered, payload, len);
}

/* =========================================================================
 * Public API
 * =========================================================================*/

xCLI_Return_t xCLI_Init(xCLI_Instance_t *xcli, xTP_Instance_t *xtp)
{
    EmbeddedCliConfig cfg;

    if (xcli == NULL || xtp == NULL) { return XCLI_ERR; }

    (void)memset(xcli, 0, sizeof(*xcli));
    xcli->xtp = xtp;

    cfg = *embeddedCliDefaultConfig();
    cfg.maxBindingCount    = (uint16_t)(XCLI_MAX_COMMANDS + 1U);
    cfg.rxBufferSize       = (uint16_t)XCLI_MAX_CMD_STR;
    cfg.cmdBufferSize      = (uint16_t)XCLI_MAX_CMD_STR;
    cfg.historyBufferSize  = 0U;
    cfg.enableAutoComplete = false;
    cfg.cliBuffer          = xcli->ecli_buf;
    cfg.cliBufferSize      = (uint16_t)sizeof(xcli->ecli_buf);

    xcli->ecli = embeddedCliNew(&cfg);
    if (xcli->ecli == NULL) {
        xCLI_Log("xCLI_Init: embeddedCliNew failed");
        return XCLI_ERR;
    }
    xcli->ecli->writeChar = xcli_nop_write_char;
    xcli->ecli->onCommand = xcli_on_unknown;

    xcli->initialized = 1U;

    /* Register our xTP handler so REQUEST frames arrive automatically. */
    s_xcli_registered = xcli;
    xTP_RegisterCommand(xtp, XCLI_XTP_CMD_ID, xCLI_XTP_Handler);

    xCLI_Log("xCLI_Init: OK  max_commands=%u  max_cmd_str=%u  frag_max=%u",
             (unsigned)XCLI_MAX_COMMANDS,
             (unsigned)XCLI_MAX_CMD_STR,
             (unsigned)XCLI_FRAG_PAYLOAD_MAX);    return XCLI_OK;
}

xCLI_Return_t xCLI_RegisterCommand(xCLI_Instance_t *xcli,
                                    uint8_t          cmd_id,
                                    const char      *name,
                                    const char      *help,
                                    xCLI_Handler_t   handler)
{
    uint8_t           i;

    if (xcli == NULL || name == NULL || handler == NULL) { return XCLI_ERR; }
    if (!xcli->initialized)                              { return XCLI_ERR_NOT_INIT; }

    /* Replace existing entry for same CMD_ID. */
    for (i = 0U; i < xcli->cmd_count; i++) {
        if (xcli->cmds[i].cmd_id == cmd_id) {
            xcli->cmds[i].handler = handler;
            (void)strncpy(xcli->cmds[i].name, name,
                          sizeof(xcli->cmds[i].name) - 1U);
            xcli->cmds[i].name[sizeof(xcli->cmds[i].name) - 1U] = '\0';
            if (help != NULL) {
                (void)strncpy(xcli->cmds[i].help, help,
                              sizeof(xcli->cmds[i].help) - 1U);
                xcli->cmds[i].help[sizeof(xcli->cmds[i].help) - 1U] = '\0';
            }
            xCLI_Log("xCLI_RegisterCommand: updated  slot=%u  cmd_id=0x%02X  name='%s'",
                     (unsigned)i, (unsigned)cmd_id, name);
            return XCLI_OK;
        }
    }

    if (xcli->cmd_count >= XCLI_MAX_COMMANDS) {
        xCLI_Log("xCLI_RegisterCommand: table full");
        return XCLI_ERR_FULL;
    }

    i = xcli->cmd_count;
    xcli->cmds[i].cmd_id  = cmd_id;
    xcli->cmds[i].handler = handler;

    (void)strncpy(xcli->cmds[i].name, name,
                  (size_t)(sizeof(xcli->cmds[i].name) - 1U));
    xcli->cmds[i].name[sizeof(xcli->cmds[i].name) - 1U] = '\0';

    if (help != NULL) {
        (void)strncpy(xcli->cmds[i].help, help,
                      (size_t)(sizeof(xcli->cmds[i].help) - 1U));
        xcli->cmds[i].help[sizeof(xcli->cmds[i].help) - 1U] = '\0';
    } else {
        xcli->cmds[i].help[0] = '\0';
    }

    /* xcli dispatches by cmd_id - no dynamic binding needed */

    xcli->cmd_count++;
    xCLI_Log("xCLI_RegisterCommand: slot=%u  cmd_id=0x%02X  name='%s'",
             (unsigned)i, (unsigned)cmd_id, name);
    return XCLI_OK;
}

xCLI_Return_t xCLI_Process(xCLI_Instance_t *xcli,
                             const uint8_t   *payload,
                             uint16_t         len)
{
    uint8_t          direction;
    uint8_t          cmd_id;
    uint8_t          seq;
    const uint8_t   *cmd_str_bytes;
    uint16_t         cmd_str_len;
    uint16_t         i;
    uint16_t         resp_len;
    uint8_t          ci;

    if (xcli == NULL || payload == NULL) { return XCLI_ERR; }
    if (!xcli->initialized)              { return XCLI_ERR_NOT_INIT; }
    if (len < XCLI_REQ_HDR_LEN)          { return XCLI_ERR_FRAME; }

    direction = payload[0];
    cmd_id    = payload[1];
    seq       = payload[2];

    xCLI_Log("<< id=0x%04X  dir=0x%02X  cmd=0x%02X  seq=%u  len=%u",
             (unsigned)XCLI_XTP_CMD_ID, (unsigned)direction,
             (unsigned)cmd_id, (unsigned)seq, (unsigned)len);

    /* Only REQUEST frames processed on server. */
    if (direction != XCLI_DIR_REQUEST) {
        xCLI_Log("   not REQUEST - ignoring");
        return XCLI_OK;
    }

    cmd_str_bytes = payload + XCLI_REQ_HDR_LEN;
    cmd_str_len   = len     - XCLI_REQ_HDR_LEN;

    /* Find registered handler. */
    ci = XCLI_MAX_COMMANDS;
    for (i = 0U; i < xcli->cmd_count; i++) {
        if (xcli->cmds[i].cmd_id == cmd_id) {
            ci = (uint8_t)i;
            break;
        }
    }

    if (ci == XCLI_MAX_COMMANDS) {
        xCLI_Log("   cmd_id=0x%02X NOT registered -> UNKNOWN_CMD", (unsigned)cmd_id);
        xcli_send_response(xcli, cmd_id, seq,
                           XCLI_STATUS_UNKNOWN_CMD, NULL, 0U);
        return XCLI_OK;
    }

    xCLI_Log("   dispatch: cmd_id=0x%02X slot=%u name='%s'",
             (unsigned)cmd_id, (unsigned)ci, xcli->cmds[ci].name);

    /* Set up request context. */
    xcli->req_ctx.xcli    = xcli;
    xcli->req_ctx.cmd_id  = cmd_id;
    xcli->req_ctx.seq     = seq;
    xcli->req_ctx.handled = 0U;
    xcli->req_ctx.status  = XCLI_STATUS_OK;
    xCLI_Ser_Init(&xcli->req_ctx.ser,
                   xcli->req_ctx.resp_buf,
                   (uint16_t)sizeof(xcli->req_ctx.resp_buf));

    /*
     * Tokenise the command string directly and call handler.
     *
     * embedded_cli has no bindings registered (staticBindings only model),
     * so we skip the embedded_cli dispatch path entirely.
     *
     * The cmd_str payload from xTP is:  "cmdname arg1 arg2 ..."
     * We copy it into a writable buffer and call embeddedCliTokenizeArgs()
     * to produce the NUL-separated token list expected by handlers.
     * The handler receives args pointing PAST the command name token,
     * consistent with how embedded_cli normally calls bindings.
     */
    {
        /* +2: one extra byte for double-NUL terminator required by tokenizer */
        static char s_args_buf[XCLI_MAX_CMD_STR + 2U];
        char       *args_ptr;
        uint16_t    copy_len;

        copy_len = (cmd_str_len < (uint16_t)(XCLI_MAX_CMD_STR + 1U))
                   ? cmd_str_len
                   : (uint16_t)(XCLI_MAX_CMD_STR);

        (void)memcpy(s_args_buf, cmd_str_bytes, (size_t)copy_len);
        s_args_buf[copy_len]      = '\0';
        s_args_buf[copy_len + 1U] = '\0'; /* double-NUL for tokenizer */

        embeddedCliTokenizeArgs(s_args_buf);

        /* Skip first token (command name), pass the rest as args */
        args_ptr = s_args_buf;
        while (*args_ptr != '\0') { args_ptr++; } /* skip cmd name */
        args_ptr++; /* advance past NUL to first arg token (or double-NUL) */

        if (xcli->cmds[ci].handler != NULL) {
            xcli->cmds[ci].handler(&xcli->req_ctx, args_ptr);
        }
    }

    /* Send response (fragmented if needed). */
    resp_len = xCLI_Ser_Finish(&xcli->req_ctx.ser);
    xcli_send_response(xcli, cmd_id, seq,
                       xcli->req_ctx.status,
                       xcli->req_ctx.resp_buf, resp_len);
    return XCLI_OK;
}

xCLI_Return_t xCLI_SendRequest(xCLI_Instance_t *xcli,
                                uint8_t          cmd_id,
                                uint8_t          seq,
                                const char      *cmd_str)
{
    static uint8_t frame[XTP_DATA_MAX_LEN]; /* static: single-instance */
    uint16_t str_len;
    uint16_t frame_len;

    if (xcli == NULL || cmd_str == NULL) { return XCLI_ERR; }
    if (!xcli->initialized)              { return XCLI_ERR_NOT_INIT; }

    str_len = 0U;
    while (cmd_str[str_len] != '\0') { str_len++; }
    if (str_len > (uint16_t)(XTP_DATA_MAX_LEN - XCLI_REQ_HDR_LEN)) {
        str_len = (uint16_t)(XTP_DATA_MAX_LEN - XCLI_REQ_HDR_LEN);
    }

    frame[0]  = XCLI_DIR_REQUEST;
    frame[1]  = cmd_id;
    frame[2]  = seq;
    frame_len = XCLI_REQ_HDR_LEN;

    (void)memcpy(&frame[frame_len], cmd_str, (size_t)str_len);
    frame_len = (uint16_t)(frame_len + str_len);

    xCLI_Log("SendRequest  cmd_id=0x%02X  seq=%u  cmd='%s'",
             (unsigned)cmd_id, (unsigned)seq, cmd_str);

#if XTP_USE_ARQ
    if (xTP_ARQ_Send(xcli->xtp, XCLI_XTP_CMD_ID, 0xFFU, frame, frame_len) != XTP_OK) {
        return XCLI_ERR;
    }
#else
    if (xTP_Send(xcli->xtp, XCLI_XTP_CMD_ID, 0xFFU, frame, frame_len) != XTP_OK) {
        return XCLI_ERR;
    }
#endif
    return XCLI_OK;
}

uint32_t xcli_token_u32(const char *tokenized_args, uint16_t pos)
{
    const char *tok = embeddedCliGetToken(tokenized_args, pos);
    if (tok == NULL) { return 0U; }
    return xcli_parse_u32(tok);
}