/*
 * xcli_config.h   xCLI compile-time configuration
 *
 * =========================================================================
 * ARCHITECTURE
 * =========================================================================
 *
 *  xTP layer  (transport):
 *    XCLI_XTP_CMD_ID  (0x0100) - REQUEST frames  (PC -> MCU).
 *    XCLI_XTP_RESP_ID (0x0101) - RESPONSE frames (MCU -> PC).
 *
 *    Two IDs are REQUIRED because xTP ARQ ACKs every data frame it receives,
 *    regardless of direction.  If both directions shared one ID:
 *      MCU sends response -> PC receives it -> PC ARQ sends ACK back
 *      -> MCU ARQ seq counter desync -> next command dropped.
 *
 *    With separate IDs:
 *      REQUEST  (0x0100): ARQ-protected PC->MCU. MCU ACKs before handling.
 *      RESPONSE (0x0101): xTP_Send (no ARQ) MCU->PC. PC is not registered
 *                         as a server for 0x0101 so it does NOT ACK back.
 *
 *  xCLI layer (inside xTP payload):
 *
 *    REQUEST  (host -> server):
 *      [DIR=0x00][CMD_ID(1B)][SEQ(1B)][cmd_string (N bytes)]
 *
 *    RESPONSE, single fragment (fields fit in one xTP frame):
 *      [DIR=0x01][CMD_ID(1B)][SEQ(1B)][STATUS(1B)][TOTAL=1][IDX=0][fields...]
 *
 *    RESPONSE, multi-fragment (fields > XCLI_FRAG_PAYLOAD_MAX bytes):
 *      Frag 0: [DIR=0x01][CMD_ID][SEQ][STATUS][TOTAL_N][IDX=0][fields chunk 0]
 *      Frag 1: [DIR=0x01][CMD_ID][SEQ][STATUS][TOTAL_N][IDX=1][fields chunk 1]
 *      ...
 *      Frag N-1: ...                                    [IDX=N-1][fields chunk N-1]
 *
 *      TOTAL_N (1 byte) = number of response frames (1..255).
 *      IDX     (1 byte) = 0-based fragment index.
 *      Fields are split only at field boundaries (never mid-field).
 *      Fragmentation is entirely within xCLI; xTP SEG is NOT used.
 *
 * =========================================================================
 * FULL RESPONSE HEADER (bytes 0..5 of every response xTP payload):
 *   [0]  DIR         = 0x01
 *   [1]  CMD_ID
 *   [2]  SEQ
 *   [3]  STATUS
 *   [4]  TOTAL_FRAGS  (1 = single-frame; >1 = multi-frame)
 *   [5]  FRAG_IDX     (0-based)
 *   [6+] fields (xCLI serial encoding)
 * =========================================================================
 *                                                  C.H
 */

#ifndef XCLI_CONFIG_H
#define XCLI_CONFIG_H

#include "M1_SysApp/xtp/xtp_config.h"

/* -------------------------------------------------------------------------
 * xTP 16-bit frame IDs for xCLI traffic.
 * Must be in application range 0x0000..0xFEFF.
 * ------------------------------------------------------------------------- */
/** xTP ID for REQUEST frames (PC -> MCU). ARQ-protected when XTP_USE_ARQ=1. */
#define XCLI_XTP_CMD_ID         ((xTP_ID_t)0x0100U)

/** xTP ID for RESPONSE frames (MCU -> PC). Plain xTP_Send, no ARQ. */
#define XCLI_XTP_RESP_ID        ((xTP_ID_t)0x0101U)

/* -------------------------------------------------------------------------
 * xCLI frame direction bytes (byte [0] of every xCLI payload)
 * ------------------------------------------------------------------------- */
#define XCLI_DIR_REQUEST        0x00U
#define XCLI_DIR_RESPONSE       0x01U

/* Header lengths */
#define XCLI_REQ_HDR_LEN        3U   /* [DIR][CMD_ID][SEQ] */
#define XCLI_RESP_HDR_LEN       6U   /* [DIR][CMD_ID][SEQ][STATUS][TOTAL][IDX] */

/* -------------------------------------------------------------------------
 * Fragmentation
 * Each fragment = one xTP frame.  Fields payload per fragment:
 *   XTP_DATA_MAX_LEN - XCLI_RESP_HDR_LEN
 * At default 8-bit LEN: 255 - 6 = 249 bytes.
 * ------------------------------------------------------------------------- */
#define XCLI_FRAG_PAYLOAD_MAX   ((uint16_t)(XTP_DATA_MAX_LEN - XCLI_RESP_HDR_LEN))

/** Total serialised response buffer (all fields before fragmentation). */
#define XCLI_MAX_RESP_TOTAL     2048U

/* -------------------------------------------------------------------------
 * Table / string sizes
 * ------------------------------------------------------------------------- */
#define XCLI_MAX_COMMANDS       16U
#define XCLI_MAX_CMD_STR        96U
#define XCLI_MAX_CMD_NAME       24U
#define XCLI_MAX_CMD_HELP       64U
#define XCLI_MAX_FIELD_NAME     16U
#define XCLI_MAX_FIELD_DATA     64U
#define XCLI_ECLI_BUF_SIZE      1024U

/* -------------------------------------------------------------------------
 * Feature switches
 * ------------------------------------------------------------------------- */
#define XCLI_USE_BUILTIN_COMMANDS   1U

/* -------------------------------------------------------------------------
 * Logging  (routes through xTP_Log)
 * ------------------------------------------------------------------------- */
#define XCLI_ENABLE_LOG         1

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif /* XCLI_CONFIG_H */
