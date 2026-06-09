/*
 * xtp_config.h
 *
 * =========================================================================
 * WIRE FORMAT
 * =========================================================================
 *
 * Every xTP frame has the following byte layout.
 * Fields marked (*) are optional >>> present only when the corresponding
 * feature flag is enabled.  Both sides MUST compile with identical flags
 *
 *   Byte(s)   Field          Present when     Description
 *   -------   -----------    ------------     ---------------------------
 *   [0]       START1         always           0xC0 (0xC0 | version nibble)
 *   [1]       START2         always           0xDE  (fixed sync byte)
 *   [2]       PROT|XFER      always           Feature-set byte (see below)
 *   [3]       SRC          * ADR_EN=1         Source node address (0x00..0xFE)
 *   [4]       DST          * ADR_EN=1         Destination address (0xFF=broadcast)
 *   [3/5]     ID_L           always           Command ID low  byte (little-endian)
 *   [4/6]     ID_H           always           Command ID high byte (always 16-bit)
 *   [5/7]     SEQ          * ARQ_EN=1         Sequence: bit7=SYN, bits6:0=seq 0..127
 *   [6/8]     SS           * SEG_EN=1         SEG stream_id (1 byte, 0..255)
 *   [...]     SI...        * SEG_EN=1         SEG chunk_idx  (LEB128, variable)
 *   [...]     ST...        * SEG_EN=1         SEG total_chunks (LEB128, 0=streaming)
 *   [...]     LEN_L          always           Payload length low byte
 *   [...]     LEN_H        * LEN_16=1         Payload length high byte
 *   [...]     DATA...        length > 0       Payload bytes (0..65535 bytes)
 *   [...]     ECC0         * ECC_EN=1         Parity byte (XOR of all DATA bytes)
 *   [...]     ECC1         * ECC_EN=1         LFSR-weighted byte (error locator)
 *   [...]     CRC...       * CRC_EN=1         CRC-16 (2B) or CRC-32 (4B), little-endian
 *   [...]     MAC0..3      * HMAC_EN=1        SipHash-2-4 truncated 32-bit tag (4 bytes)
 *   [-2]      STOP1          always           0xDA (fixed stop byte)
 *   [-1]      STOP2          always           0xED (fixed stop byte)
 *
 * Minimum frame (no optional fields): 7 bytes
 *   START1 + START2 + PROT|XFER + ID_L + ID_H + LEN + STOP1 + STOP2
 *
 * =========================================================================
 * PROT|XFER BYTE  (byte [2])
 * =========================================================================
 *
 * Encodes which optional fields are present.  MUSTT match on both sides
 *
 *   PROT nibble [7:4] - data integrity and authentication:
 *
 *   bit 7  CRC_EN   : 1 = CRC field present after ECC (or DATA if ECC off)
 *   bit 6  CRC_TYPE : 0 = CRC-16/XMODEM, 2 bytes, poly 0x1021, init 0x0000
 *                     1 = CRC-32,         4 bytes, poly 0x04C11DB7 (non-reflected)
 *   bit 5  ECC_EN   : 1 = ECC0+ECC1 appended after DATA
 *                         Corrects 1-byte errors, detects 2+.  Max 255 B payload.
 *                         Requires LEN_16=0.
 *   bit 4  HMAC_EN  : 1 = 4-byte SipHash-2-4 tag appended after CRC
 *                         Covers TM byte through last CRC byte (inclusive).
 *
 *   XFER nibble [3:0] - transport features:
 *
 *   bit 3  LEN_16   : 0 = LEN field is 8-bit  (max payload  255 bytes)
 *                     1 = LEN field is 16-bit (max payload 65535 bytes)
 *                         Requires ECC_EN=0.
 *   bit 2  SEG_EN   : 1 = SEG header present before LEN:
 *                         stream_id (1B) + chunk_idx (LEB128) + total (LEB128)
 *                         total=0 means streaming - deliver per chunk, no buffer.
 *   bit 1  ARQ_EN   : 1 = SEQ byte present after ID (Stop-and-Wait + NACK).
 *                         bit7=SYN (session sync), bits6:0=sequence 0..127.
 *   bit 0  ADR_EN   : 1 = SRC+DST address bytes present after PROT|XFER.
 *
 * ID is ALWAYS 16-bit, little-endian.  No flag needed.
 * RESERVED IDs: 0xFF00..0xFFFF (ARQ control):
 *   0xFFFE=ACK  0xFFFD=NACK  0xFFFC=RST
 * Application IDs must stay in 0x0000..0xFEFF.
 *                                                  C.H
 */

#ifndef XTP_CONFIG_H
#define XTP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Protocol version
 * Lower nibble of START1 byte (0xC0 | version).
 * Increment on any backward-incompatible wire format change.
 * Default: 0
 * =========================================================================*/
#define XTP_PROTOCOL_VERSION  0U

/* =========================================================================
 * Frame synchronisation bytes  (excluded from CRC and MAC coverage)
 * =========================================================================*/
#define XTP_START1_BYTE     (0xC0U | (XTP_PROTOCOL_VERSION & 0x0FU))
#define XTP_START2_BYTE     0xDEU
#define XTP_STOP1_BYTE      0xDAU
#define XTP_STOP2_BYTE      0xEDU

/** Destination address accepted by every node. */
#define XTP_BROADCAST_ADDR  0xFFU

/* =========================================================================
 * CRC PROT bit 7 (CRC_EN) + bit 6 (CRC_TYPE)
 * Detects data corruption in the frame header and payload.
 * CRC covers: PROT|XFER byte through the last ECC byte (inclusive).
 * START, STOP, and MAC bytes are excluded from CRC coverage.
 * Default: enabled, CRC-16/XMODEM, table-driven
 * =========================================================================*/
#define XTP_USE_CRC         1   /* 1=CRC enabled  0=disabled (no corruption detection!) */

/** 16 = CRC-16/XMODEM (2 bytes)  |  32 = CRC-32 (4 bytes)
 *  Defined unconditionally so overhead macros compile even when CRC=0. */
#define XTP_USE_CRC_TYPE    16

/** 1 = 256-entry lookup table (~512 B ROM for CRC-16, ~1 KB for CRC-32, fast)
 *  0 = bit-serial computation  (no ROM cost, ~8X slower) */
#define XTP_CRC_USE_TABLE   1

/* =========================================================================
 * ECC  -  PROT bit 5
 * CA-based single-byte error correction (Chowdhury et al., 1996).
 * Appends two check bytes (ECC0 + ECC1) after the DATA field.
 *   ECC0 = XOR parity of all data bytes (detects odd-count bit flips).
 *   ECC1 = LFSR-weighted XOR (primitive poly x^8+x^4+x^3+x^2+1, tap 0x8E).
 *          Together they locate the corrupted byte position.
 * Corrects exactly 1 corrupted byte per frame.
 * Detects (but cannot correct) 2+ simultaneous byte errors.
 * CONSTRAINT: payload must be <= 255 bytes.  Requires LEN_16=0.
 * Default: enabled
 * =========================================================================*/
#define XTP_USE_ECC         1   /* 1=ECC enabled  0=disabled */

/* =========================================================================
 * HMAC  -  PROT bit 4
 * 4-byte message authentication tag using SipHash-2-4 (Aumasson & Bernstein).
 * Truncated from 64-bit output to 32 bits (lower word).
 * MAC input order: header bytes (TM..LEN) || DATA bytes || ECC bytes || CRC bytes.
 * Including CRC in the MAC input prevents payload-swap attacks where an
 * attacker substitutes payload and recomputes CRC without knowing the key.
 * Appended as 4 bytes after the CRC field (or after DATA/ECC if CRC=off).
 * Default: disabled
 * =========================================================================*/
#define XTP_USE_HMAC        0   /* 1=HMAC enabled  0=disabled */
#if XTP_USE_HMAC
/** 128-bit SipHash key (16 bytes).
 *  Change this for every deployment.  For production, read from provisioned
 *  flash at runtime rather than embedding a literal here. */
#define XTP_HMAC_KEY    { 0x2B,0x7E,0x15,0x16, 0x28,0xAE,0xD2,0xA6, \
                          0xAB,0xF7,0x15,0x88, 0x09,0xCF,0x4F,0x3C }
#endif

/* =========================================================================
 * ID field  -  always 16-bit (uint16_t), little-endian.  No flag.
 * RESERVED range 0xFF00..0xFFFF is used internally by ARQ:
 *   0xFFFE = ACK  |  0xFFFD = NACK  |  0xFFFC = RST
 * Application commands must use IDs in 0x0000..0xFEFF only.
 * =========================================================================*/

/* =========================================================================
 * LEN field width  -  XFER bit 3
 *   LEN_16 = 0 : 8-bit  LEN field -> max payload  255 bytes  (default)
 *   LEN_16 = 1 : 16-bit LEN field -> max payload 65535 bytes
 *                Requires ECC_EN=0 (ECC only supports up to 255-byte blocks).
 * Default: 0 (8-bit LEN)
 * =========================================================================*/
#define XTP_USE_LEN_16      0   /* 1=16-bit LEN  0=8-bit LEN */
#if XTP_USE_LEN_16 && XTP_USE_ECC
#error "XTP_USE_LEN_16=1 requires XTP_USE_ECC=0  (ECC max block size is 255 bytes)"
#endif

/* =========================================================================
 * SEG  -  XFER bit 2
 * Segmentation layer for transfers larger than one frame.
 * Inserts a variable-length header before LEN:
 *   stream_id     (1 byte)  : identifies the logical stream (0..255).
 *   chunk_idx     (LEB128)  : zero-based chunk index (unbounded, uint32_t).
 *   total_chunks  (LEB128)  : total chunks; 0 = streaming mode.
 *
 * Two operating modes:
 *   Known-total  (total > 0): receiver buffers all chunks in XTP_SEG_BUF_SIZE
 *                             and fires on_complete once the last chunk arrives.
 *   Streaming    (total = 0): on_complete fires once per chunk immediately,
 *                             no reassembly buffer needed, unlimited size.
 * Default: disabled
 * =========================================================================*/
#define XTP_USE_SEG         0   /* 1=SEG enabled  0=disabled */
#if XTP_USE_SEG

/** Maximum chunks in one known-total transfer (streaming has no limit). */
#define XTP_SEG_MAX_CHUNKS  128U

/** Maximum concurrent reassembly streams. */
#define XTP_SEG_MAX_STREAMS 1U

/* Reassembly buffer per stream (bytes) - static MCU RAM allocation.
 * Set this according to available RAM, NOT XTP_DATA_MAX_LEN.
 *
 * Rule   : XTP_SEG_BUF_SIZE >= size of the largest known-total transfer.
 * Note   : Streaming mode (total=0) uses NO buffer - ize is unlimited.
 * Note   : uint32_t cast prevents silent overflow when value > 65535.
 *
 * Example sizes:
 *   255 * 128 = 32640 B -> 32 KB  (default - fits STM32F4/F7)
 *   255 *  16 =  4080 B ->  4 KB  (tight RAM: STM32F0/G0/L0)
 *   512 *  64 = 32768 B -> 32 KB  (larger chunks, same RAM) */
#define XTP_SEG_CHUNK_SIZE  255U          /* bytes reserved per chunk slot */
#define XTP_SEG_BUF_SIZE    ((uint32_t)(XTP_SEG_MAX_CHUNKS * XTP_SEG_CHUNK_SIZE))

/** Incomplete-stream timeout (ms): discard stream if no new chunk arrives
 *  within this window.  Only applies to known-total mode. */
#define XTP_SEG_TIMEOUT_MS  3000U

#endif /* XTP_USE_SEG */

/* =========================================================================
 * ARQ  -  XFER bit 1
 * Blocking ACK automatic repeat request .
 * xTP_ARQ_Send() blocks until ACK received or retries exhausted.
 * ACK is sent immediately on data frame reception (no deferred ctrl).
 * No Poll function needed.
 *
 * Inserts a 1-byte SEQ field in the frame HEADER (not in the payload):
 *   bits 6:0 seq: sequence number 0..127, wraps at 128 (debug only).
 *   bit 7: unused (was SYN, now always 0).
 * SEQ increments after each ACKed frame - for debug/logging only,
 * no duplicate detection is performed.
 *
 * ACK and NACK carry the acknowledged seq as a 1-byte payload.
 * Control frame IDs (always 16-bit): ACK=0xFFFE  NACK=0xFFFD  RST=0xFFFC
 * Default: enabled
 * =========================================================================*/
#define XTP_USE_ARQ         1   /* 1=ARQ enabled  0=disabled (fire-and-forget) */
#if XTP_USE_ARQ

/** ACK wait timeout (ms): retransmit unACKed frame after this interval. */
#define XTP_ARQ_TIMEOUT_MS  300U

/** Max retransmit attempts before the frame is marked FAILED and dropped. */
#define XTP_ARQ_MAX_RETRIES 3U

#endif /* XTP_USE_ARQ */

/* =========================================================================
 * Addressing  -  XFER bit 0
 * Inserts SRC (1B) and DST (1B) fields after PROT|XFER.
 * Enables multi-node routing on a shared physical bus.
 * Frames addressed to a different node are silently discarded by the decoder.
 * 0xFF is the broadcast address - accepted by every node.
 * Default: disabled (point-to-point link)
 * =========================================================================*/
#define XTP_USE_TARGET      0   /* 1=addressing enabled  0=disabled */

/* =========================================================================
 * Payload buffer
 * Maximum payload bytes per frame (statically allocated on MCU).
 *   LEN_16=0: maximum is 255 (8-bit LEN field hard limit).
 *   LEN_16=1: set this to the largest single frame payload expected.
 *             Lower values save RAM.  Hard maximum is 65535.
 * Default: 255 (8-bit mode)
 * =========================================================================*/
#if XTP_USE_LEN_16
#define XTP_DATA_MAX_LEN    5120U   /* bytes; adjust to taste, max 65535 */
#else
#define XTP_DATA_MAX_LEN    255U
#endif

/* =========================================================================
 * Inter-byte timeout
 * If no byte arrives within this many milliseconds while a frame is being
 * received, the partial frame is discarded and the FSM resets to START1.
 * Prevents a permanently stuck decoder caused by noise or a lost stop byte.
 * Default: 300 ms
 * =========================================================================*/
#define XTP_TIMEOUT_MS      300U

/* =========================================================================
 * Command dispatch table
 * Maximum number of IDs registered via xTP_RegisterCommand().
 * Each entry costs sizeof(xTP_Command_t) bytes of RAM.
 * Default: 16
 * =========================================================================*/
#define XTP_MAX_COMMANDS    16U

/* =========================================================================
 * TX / RX locks
 *   Bare-metal single-loop (all calls from one task): set both to 0.
 *   RTOS / multi-thread: set both to 1 and implement the four callbacks
 *     xTP_Port_TxLock / TxUnlock / RxLock / RxUnlock in xtp_port.c.
 *     FreeRTOS: xSemaphoreTake / xSemaphoreGive on two mutexes.
 *     Zephyr  : k_mutex_lock / k_mutex_unlock.
 *     POSIX   : pthread_mutex_lock / pthread_mutex_unlock.
 *
 * Default: 1 (safe default - set to 0 only if single-threaded bare-metal)
 * =========================================================================*/
#define XTP_USE_TX_LOCK     1   /* 1=TX path mutex-protected  0=no lock */
#define XTP_USE_RX_LOCK     1   /* 1=RX path mutex-protected  0=no lock */

/* =========================================================================
 * Logging
 * When enabled, xTP_Log() sends formatted debug strings.
 * Messages are prefixed "[x] " and terminated "\r\n".
 * Disable in production to reduce code size, ROM, and UART traffic.
 * Default: enabled
 * =========================================================================*/
#define XTP_ENABLE_LOG      1            /* 1=logging enabled  0=disabled */
#define XTP_LOG_BUFFER_SIZE 256U        /* maximum characters per log message */

/* =========================================================================
 * NULL
 * =========================================================================*/
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef __cplusplus
}
#endif
#endif /* XTP_CONFIG_H */