/************************************************
 *  @file     : rparam_server.h
 *  @brief    : RPARAM server for SAMV71 / FreeRTOS
 *              Packet dispatch model — no dedicated socket.
 *              Caller (app_csp.c) receives packets on port
 *              GS_CSP_PORT_RPARAM (7) and passes them to
 *              rparam_server_handle_packet().
 ************************************************/

#ifndef RPARAM_SERVER_H
#define RPARAM_SERVER_H

#include <param/types.h>
#include <param/table.h>
#include <param/serialize.h>
#include <param/internal/rparam.h>
#include <gs_support/gs_error.h>
#include <csp/csp.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of parameter tables served */
#define RPARAM_MAX_TABLES   8

/* Maximum address entries in a single GET-list request.
 * GS_RPARAM_QUERY_MAX_PAYLOAD = 180 bytes / 2 bytes per addr = 90 */
#define RPARAM_MAX_ADDR_LIST    90

/* Max payload per GET-full-table fragment (bytes).
 * Must fit inside a CSP buffer (conf.buffer_data_size = 256).
 * Header = sizeof(gs_rparam_query_t) without payload ~ 8 bytes. */
#define RPARAM_MAX_FRAGMENT_PAYLOAD  180

/**
 * Server configuration — set once before rparam_server_init().
 */
typedef struct {
    gs_param_table_instance_t *tables[RPARAM_MAX_TABLES];
    uint8_t table_count;
} rparam_server_config_t;

/**
 * Server handle — allocate statically in application.
 * Example:
 *   static rparam_server_t g_rparam;
 */
typedef struct {
    rparam_server_config_t config;
    bool initialized;
} rparam_server_t;

/**
 * Initialize the RPARAM server.
 * Call once before the CSP task starts.
 *
 * @param server   pointer to static rparam_server_t
 * @param config   pointer to filled rparam_server_config_t
 * @return GS_OK on success
 */
gs_error_t rparam_server_init(rparam_server_t *server,
                              const rparam_server_config_t *config);

/**
 * Handle one incoming RPARAM packet.
 * Call this from inside the CSP server loop when dport == GS_CSP_PORT_RPARAM.
 *
 * Ownership: pkt is consumed (freed) by this function.
 *
 * @param server  initialized server handle
 * @param conn    active CSP connection
 * @param pkt     received CSP packet (will be freed internally)
 */
void rparam_server_handle_packet(rparam_server_t *server,
                                 csp_conn_t *conn,
                                 csp_packet_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* RPARAM_SERVER_H */