/************************************************
 *  @file     : rparam_server.c
 ************************************************/

#include "rparam_server.h"
#include <param/internal/types.h>
#include <param/internal/serialize.h>
#include <gs_support/fletcher.h>
#include <csp/csp_endian.h>
#include <string.h>
#include "M1_SysApp/dmesg/dmesg.h"

/* ============================================================================
 * Internal
 * ============================================================================ */
#define RPARAM_GET      0x00
#define RPARAM_REPLY    0x55
#define RPARAM_SET      0xFF
#define RPARAM_SET_OK   0x01
#define RPARAM_TABLE    0x44
#define RPARAM_ERROR    0xFF

/* ============================================================================
 * Internal helpers
 * ============================================================================ */

static gs_param_table_instance_t *get_table(rparam_server_t *srv, uint8_t id)
{
    if (id >= srv->config.table_count) return NULL;
    return srv->config.tables[id];
}

static void send_error(csp_conn_t *conn)
{
    csp_packet_t *pkt = csp_buffer_get(1);
    if (pkt) {
        pkt->data[0] = RPARAM_ERROR;
        pkt->length  = 1;
        if (!csp_send(conn, pkt, 0)) {
            csp_buffer_free(pkt);
        }
    }
}

static bool validate_checksum(gs_param_table_instance_t *table, uint16_t checksum)
{
    if (checksum == GS_RPARAM_MAGIC_CHECKSUM) return true;
    return (checksum == gs_param_table_checksum_le(table) ||
            checksum == gs_param_table_checksum_be(table));
}

/* ============================================================================
 * Handle GET
 * ============================================================================ */
static void handle_get(csp_conn_t *conn, gs_rparam_query_t *query,
                       rparam_server_t *srv)
{
    gs_param_table_instance_t *table = get_table(srv, query->table_id);
    if (!table) {
        Dmesg_Write("[RPARAM] GET: invalid table_id");
        send_error(conn);
        return;
    }

    const uint16_t length   = csp_ntoh16(query->length);
    const uint16_t checksum = csp_ntoh16(query->checksum);

    if (!validate_checksum(table, checksum)) {
        Dmesg_Write("[RPARAM] GET: bad checksum");
        send_error(conn);
        return;
    }

    Dmesg_Write("[RPARAM] GET: Accepted request");

    /* ---- Full table request (length == 0) ---- */
    if (length == 0) {
        /* Dry-run to get total byte count */
        unsigned int total_bytes = 0;
        unsigned int param_pos   = 0;
        gs_param_serialize_full_table(table, &param_pos,
                                      GS_PARAM_SF_DRY_RUN,
                                      NULL, (unsigned int)-1, &total_bytes);

        const unsigned int max_pay = RPARAM_MAX_FRAGMENT_PAYLOAD;
        unsigned int pkts_needed   = (total_bytes + max_pay - 1) / max_pay;
        if (pkts_needed == 0) pkts_needed = 1;

        param_pos = 0;

        for (unsigned int p = 0; p < pkts_needed; p++) {
            csp_packet_t *rsp = csp_buffer_get(sizeof(gs_rparam_query_t) + max_pay);
            if (!rsp) {
                Dmesg_Write("[RPARAM] GET: no buffer");
                break;
            }

            gs_rparam_query_t *reply = (gs_rparam_query_t *)rsp->data;
            reply->action   = RPARAM_REPLY;
            reply->table_id = query->table_id;
            reply->checksum = csp_hton16(checksum);

            /* seq/total: single packet → both 0
             *            multi packet → seq 0..N-1, last seq==total==N */
            if (pkts_needed == 1) {
                reply->seq   = csp_hton16(0);
                reply->total = csp_hton16(0);
            } else if (p == pkts_needed - 1) {
                reply->seq   = csp_hton16(pkts_needed);
                reply->total = csp_hton16(pkts_needed);
            } else {
                reply->seq   = csp_hton16(p);
                reply->total = csp_hton16(pkts_needed);
            }

            unsigned int buf_pos = 0;
            gs_param_serialize_full_table(table, &param_pos,
                                          F_TO_BIG_ENDIAN,
                                          reply->payload.packed,
                                          max_pay, &buf_pos);

            reply->length = csp_hton16(buf_pos);
            rsp->length   = RPARAM_QUERY_LENGTH(reply, buf_pos);

            if (!csp_send(conn, rsp, 0)) {
                csp_buffer_free(rsp);
                Dmesg_Write("[RPARAM] GET: send failed");
                break;
            }

            /* Yield between fragments so CSP router can flush */
            vTaskDelay(pdMS_TO_TICKS(5));
        }
        return;
    }

    /* ---- Specific address list (length > 0) ---- */
    const unsigned int addr_count = length / sizeof(uint16_t);
    if (addr_count > RPARAM_MAX_ADDR_LIST) {
        Dmesg_Write("[RPARAM] GET: addr list too long");
        send_error(conn);
        return;
    }

    /* Static buffer — no alloca */
    uint16_t addrs[RPARAM_MAX_ADDR_LIST];
    for (unsigned int i = 0; i < addr_count; i++) {
        addrs[i] = csp_ntoh16(query->payload.addr[i]);
    }

    csp_packet_t *rsp = csp_buffer_get(sizeof(gs_rparam_query_t) + RPARAM_MAX_FRAGMENT_PAYLOAD);
    if (!rsp) {
        Dmesg_Write("[RPARAM] GET: no buffer");
        send_error(conn);
        return;
    }

    gs_rparam_query_t *reply = (gs_rparam_query_t *)rsp->data;
    reply->action   = RPARAM_REPLY;
    reply->table_id = query->table_id;
    reply->checksum = csp_hton16(checksum);
    reply->seq      = csp_hton16(0);
    reply->total    = csp_hton16(0);

    unsigned int buf_pos  = 0;
    unsigned int param_pos = 0;
    gs_error_t err = gs_param_serialize_list(table, addrs, addr_count,
                                             &param_pos, F_TO_BIG_ENDIAN,
                                             reply->payload.packed,
                                             RPARAM_MAX_FRAGMENT_PAYLOAD,
                                             &buf_pos);
    if (err != GS_OK) {
        csp_buffer_free(rsp);
        send_error(conn);
        return;
    }

    reply->length = csp_hton16(buf_pos);
    rsp->length   = RPARAM_QUERY_LENGTH(reply, buf_pos);

    if (!csp_send(conn, rsp, 0)) {
        csp_buffer_free(rsp);
    }
}

/* ============================================================================
 * Handle SET
 * ============================================================================ */
static void handle_set(csp_conn_t *conn, gs_rparam_query_t *query,
                       rparam_server_t *srv)
{
    gs_param_table_instance_t *table = get_table(srv, query->table_id);
    if (!table) {
        Dmesg_Write("[RPARAM] SET: invalid table_id");
        send_error(conn);
        return;
    }

    const uint16_t length   = csp_ntoh16(query->length);
    const uint16_t checksum = csp_ntoh16(query->checksum);

    if (!validate_checksum(table, checksum)) {
        Dmesg_Write("[RPARAM] SET: bad checksum");
        send_error(conn);
        return;
    }

    gs_error_t err = gs_param_deserialize(table, query->payload.packed,
                                          length, F_FROM_BIG_ENDIAN);
    if (err != GS_OK) {
        Dmesg_Write("[RPARAM] SET: deserialize failed");
        send_error(conn);
        return;
    }
    
    Dmesg_Write("[RPARAM] SET: Accepted request");

    /* Echo back SET_OK with same payload */
    csp_packet_t *rsp = csp_buffer_get(sizeof(gs_rparam_query_t) + length);
    if (!rsp) {
        send_error(conn);
        return;
    }

    gs_rparam_query_t *reply = (gs_rparam_query_t *)rsp->data;
    reply->action   = RPARAM_SET_OK;
    reply->table_id = query->table_id;
    reply->length   = csp_hton16(length);
    reply->checksum = csp_hton16(checksum);
    reply->seq      = 0;
    reply->total    = 0;
    memcpy(reply->payload.packed, query->payload.packed, length);
    rsp->length = RPARAM_QUERY_LENGTH(reply, length);

    if (!csp_send(conn, rsp, 0)) {
        csp_buffer_free(rsp);
    }
}

/* ============================================================================
 * Handle TABLE spec request
 * ============================================================================ */
static void handle_table(csp_conn_t *conn, gs_rparam_query_t *query,
                         rparam_server_t *srv)
{
    gs_param_table_instance_t *table = get_table(srv, query->table_id);
    if (!table || !table->rows || table->row_count == 0) {
        Dmesg_Write("[RPARAM] TABLE: invalid table");
        send_error(conn);
        return;
    }
    
    Dmesg_Write("[RPARAM] TABLE: Accepted request");

    const size_t table_size = sizeof(*table->rows) * table->row_count;
    int res = csp_sfp_send(conn, table->rows, table_size, 1000, 10000);
    if (res != CSP_ERR_NONE) {
        Dmesg_Write("[RPARAM] TABLE: sfp_send failed");
    }
}

/* ============================================================================
 * Public API
 * ============================================================================ */

gs_error_t rparam_server_init(rparam_server_t *server,
                              const rparam_server_config_t *config)
{
    if (!server || !config) return GS_ERROR_ARG;
    if (config->table_count == 0 || config->table_count > RPARAM_MAX_TABLES) {
        return GS_ERROR_ARG;
    }
    for (int i = 0; i < config->table_count; i++) {
        if (!config->tables[i]) return GS_ERROR_ARG;
    }

    memset(server, 0, sizeof(*server));
    memcpy(&server->config, config, sizeof(*config));
    server->initialized = true;

    Dmesg_Write("[RPARAM] server initialized");
    return GS_OK;
}

void rparam_server_handle_packet(rparam_server_t *server,
                                 csp_conn_t *conn,
                                 csp_packet_t *pkt)
{
    if (!server || !server->initialized || !conn || !pkt) {
        if (pkt) csp_buffer_free(pkt);
        return;
    }

    /* Minimum valid packet: header only (no payload) */
    if (pkt->length < (int)sizeof(gs_rparam_query_t) - sizeof(gs_rparam_query_payload_t)) {
        Dmesg_Write("[RPARAM] packet too small");
        csp_buffer_free(pkt);
        send_error(conn);
        return;
    }

    gs_rparam_query_t *query = (gs_rparam_query_t *)pkt->data;

    switch (query->action) {
        case RPARAM_GET:
            handle_get(conn, query, server);
            break;
        case RPARAM_SET:
            handle_set(conn, query, server);
            break;
        case RPARAM_TABLE:
            handle_table(conn, query, server);
            break;
        default:
            Dmesg_Write("[RPARAM] unknown action");
            send_error(conn);
            break;
    }

    csp_buffer_free(pkt);
}