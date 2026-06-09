/*
 * rgosh_server.c
 *
 *  Created on: February 2026
 *      Author: CAO HIEU
 */
/*
        __        __  _    ____  _   _ ___ _   _  ____     __
        \ \      / / / \  |  _ \| \ | |_ _| \ | |/ ___|   | |
        \ \ /\ / / / _ \ | |_) |  \| || ||  \| | |  _     | |
        \ V  V / / ___ \|  _ <| |\  || || |\  | |_| |     |_|
        \_/\_/ /_/   \_\_| \_\_| \_|___|_| \_|\____|      ( )

  =====================================================================
   !! BUILD FLAG REQUIRED !!
  =====================================================================

  This module uses nanopb with 16-bit field descriptors.

  The following compiler flag MUST be added to the project:

      -DPB_FIELD_16BIT

  MPLAB X IDE:
    Project Properties -> XC32 (Global Options) -> xc32-gcc
    -> Additional options -> add: -DPB_FIELD_16BIT

  CMake:
    target_compile_definitions(<target> PRIVATE PB_FIELD_16BIT)

  Makefile:
    CFLAGS += -DPB_FIELD_16BIT

  WITHOUT THIS FLAG:
    pb_size_t = uint8_t  -> GoshRequest struct layout mismatch
    -> pb_decode() fails with "invalid data_size"
    -> all incoming CSP/RGOSH commands silently dropped

  =====================================================================
*/

#include "rgosh_server.h"

#include <string.h>
#include <csp/csp.h>
#include <csp/arch/csp_malloc.h>

#include "rgosh_serialize.h"
#include "gosh.pb.h"

#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "M1_SysApp/xlog/xlog.h"

/* ================================================================
 *  Init
 * ================================================================ */
xerr_t rgosh_init(void)
{
    if (get_RGOSH_CliPointer() == NULL) {
        xlog("[RGOSH] CLI not ready - call SystemCLI_Init() first\r\n");
        return XERR_IO;
    }
    xlog("[RGOSH] ready (port %d)\r\n", GS_CSP_PORT_RGOSH);
    return XERR_OK;
}

/* ================================================================
 *  Internal
 * ================================================================ */
static void execute_cmd(const char *cmd_str, char *out_buf,
                        size_t out_max, size_t *out_len)
{
    EmbeddedCli *cli = get_RGOSH_CliPointer();
    if (!cli || !cmd_str || cmd_str[0] == '\0') {
        if (out_len) *out_len = 0;
        return;
    }

    for (const char *p = cmd_str; *p; p++)
        embeddedCliReceiveChar(cli, *p);
    embeddedCliReceiveChar(cli, '\r');

    Cli_RGOSH_CaptureStart(out_buf, out_max);
    embeddedCliProcess(cli);
    Cli_RGOSH_CaptureStop(out_len);
}

/* ================================================================
 *  Public
 * ================================================================ */
void rgosh_handle_conn(csp_conn_t *conn)
{
    /* ---- 1. Receive SFP frame ---- */
    void *recv_data = NULL;
    int   recv_len  = 0;

    if (csp_sfp_recv(conn, &recv_data, &recv_len, 5000) != CSP_ERR_NONE) {
        xlog("[RGOSH] sfp_recv failed\r\n");
        return;
    }
    
    /* ---- 2. Deserialize ---- */
    GoshRequest req = GoshRequest_init_zero;
    xerr_t xr = gs_rgosh_deserialize_request((uint8_t *)recv_data,
                                              (uint32_t)recv_len, &req);
    csp_free(recv_data);

    if (xr != XERR_OK) {
        xlog("[RGOSH] deserialize failed\r\n");
        return;
    }
    if (req.which_command != GoshRequest_run_tag) {
        xlog("[RGOSH] unsupported cmd type=%u\r\n", req.which_command);
        return;
    }

    xlog("[RGOSH] cmd: '%s'\r\n", req.command.run.command);

    /* ---- 3. Execute ---- */
    static char out_buf[RGOSH_OUT_BUF_SIZE];
    size_t out_len = 0;
    execute_cmd(req.command.run.command, out_buf, sizeof(out_buf), &out_len);

    xlog("[RGOSH] out: %u bytes\r\n", (unsigned)out_len);

    /* ---- 4. Build response ---- */
    GoshResponse resp = GoshResponse_init_zero;
    resp.id             = req.id;
    resp.which_response = GoshResponse_command_tag;

    CommandResponse *cr = &resp.response.command;
    cr->return_code     = 0;
    cr->completion_flag = true;
    cr->results_count   = 0;

    if (out_len > 0) {
        cr->has_std_out = true;
        strncpy(cr->std_out, out_buf, sizeof(cr->std_out) - 1);
        cr->std_out[sizeof(cr->std_out) - 1] = '\0';
    }

    /* ---- 5. Serialize & send ---- */
    static uint8_t resp_buf[GoshResponse_size];
    uint32_t resp_len = sizeof(resp_buf);

    if (gs_rgosh_serialize_response(&resp, resp_buf, &resp_len) != XERR_OK) {
        xlog("[RGOSH] serialize failed\r\n");
        return;
    }

    if (csp_sfp_send(conn, resp_buf, (int)resp_len,
                     GS_RGOSH_MTU_SIZE, 5000) != CSP_ERR_NONE) {
        xlog("[RGOSH] sfp_send failed\r\n");
        return;
    }

    xlog("[RGOSH] sent %lu B ok\r\n", (unsigned long)resp_len);
}