/************************************************
 *  @file     : app_csp.c
 *  @date     : February 2026
 *  @author   : CAO HIEU
 *  @version  : 3.1.0
 ************************************************/

#include "app_csp.h"
#include "rparam_server.h"
#include "exp_rparam_table.h"
#include "M1_SysApp/dmesg/dmesg.h"

#include <string.h>
#include <stdio.h>

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include "csp/drivers/can_mcan.h"
#include "peripheral/mcan/plib_mcan1.h"

#include "hwcsp_cfg.h"
#include <param/internal/types.h>

#include "rgosh_server.h"
/* ---- Port definitions ---- */
#define CSP_PORT_APP    10

/* ================================================================
 *  RPARAM Service
 * ================================================================ */ 
gs_param_table_instance_t g_tables[5];
static rparam_server_t g_rparam;

/* ================================================================
 *  Raw CAN send CLI Test
 * ================================================================ */
bool CAN_RawSend(uint32_t canId, uint8_t *data, uint8_t dlc, bool extended)
{
    MCAN_TX_BUFFER txBuf;
    memset(&txBuf, 0, sizeof(txBuf));

    if (dlc > 8) dlc = 8;

    if (extended) {
        txBuf.id  = canId & 0x1FFFFFFFU;
        txBuf.xtd = 1;
    } else {
        txBuf.id  = (canId & 0x7FFU) << 18;
        txBuf.xtd = 0;
    }

    txBuf.dlc = dlc;
    if (data && dlc > 0) memcpy(txBuf.data, data, dlc);

    if (MCAN1_TxFifoFreeLevelGet() == 0) return false;
    return MCAN1_MessageTransmitFifo(1, &txBuf);
}

/* ================================================================
 *  CSP Server loop
 * ================================================================ */
static void csp_server_loop(void)
{
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);
    if (sock == NULL) {
        Dmesg_Write("CSP: socket() failed");
        return;
    }

    csp_bind(sock, CSP_ANY);
    csp_listen(sock, 10);
    Dmesg_Write("CSP: server listening");

    while (1) {
        csp_conn_t *conn = csp_accept(sock, 1000);
        if (conn == NULL) continue;

        int dport = csp_conn_dport(conn);

        /* ---- RGOSH: SFP-based ---- */
        if (dport == GS_CSP_PORT_RGOSH) {
            rgosh_handle_conn(conn);
            csp_close(conn);
            continue;
        }


        csp_packet_t *pkt;

        while ((pkt = csp_read(conn, 200)) != NULL) {

            switch (dport) {

                /* ---- RPARAM handler (port 7) ---- */
                case GS_CSP_PORT_RPARAM:
                    rparam_server_handle_packet(&g_rparam, conn, pkt);
                    break;

                /* ---- Test echo handler ---- */
                case CSP_PORT_APP: {
                    char buf[64];
                    snprintf(buf, sizeof(buf),
                             "CSP RX port=%d len=%u", dport, pkt->length);
                    Dmesg_Write(buf);

                    csp_packet_t *reply = csp_buffer_get(pkt->length);
                    if (reply != NULL) {
                        memcpy(reply->data, pkt->data, pkt->length);
                        reply->length = pkt->length;
                        if (!csp_send(conn, reply, 1000)) {
                            csp_buffer_free(reply);
                        }
                    }
                    csp_buffer_free(pkt);
                    break;
                }

                /* ---- Built-in CSP services ---- */
                default:
                    csp_service_handler(conn, pkt);
                    break;
            }
        }

        csp_close(conn);
    }
}

/* ================================================================
 *  App_CSPTask
 * ================================================================ */
const osThreadAttr_t csp_attr = {
    .name       = "CSP",
    .stack_size = configMINIMAL_STACK_SIZE * 4,
    .priority   = configMAX_PRIORITIES - 3
};

void App_CSPTask(void *param)
{
    char buf[64];

    /* Step 1: Init parameter tables */
    if (data_tables_init_all(g_tables) != GS_OK) {
        Dmesg_Write("RPARAM: table init FAILED");
        vTaskDelete(NULL);
        return;
    }

    /* Step 2: Init RPARAM server */
    rparam_server_config_t rparam_cfg = {
        .tables = {
            &g_tables[0],
            &g_tables[1],
            &g_tables[2],
            &g_tables[3],
            &g_tables[4],
        },
        .table_count = 5,
    };

    if (rparam_server_init(&g_rparam, &rparam_cfg) != GS_OK) {
        Dmesg_Write("RPARAM: server init FAILED");
        vTaskDelete(NULL);
        return;
    }

    /* Step 3: Init CSP */
    csp_conf_t conf;
    csp_conf_get_defaults(&conf);
    conf.address          = CSP_MY_ADDRESS;
    conf.buffer_data_size = 1024;
    conf.buffers          = 32;
    conf.fifo_length      = 32;
    conf.hostname         = CSP_CONF_HOSTNAME;
    conf.model            = CSP_CONF_MODEL;
    conf.revision         = CSP_CONF_REVISION;

    if (csp_init(&conf) != CSP_ERR_NONE) {
        Dmesg_Write("CSP: init FAILED");
        vTaskDelete(NULL);
        return;
    }
    Dmesg_Write("CSP: initialized");

    /* Step 4: CAN interface */
    csp_iface_t *can_iface = NULL;
    int res = csp_can_mcan_open_and_add_interface("CAN", CSP_CAN_PROMISC, &can_iface);
    if (res != CSP_ERR_NONE) {
        snprintf(buf, sizeof(buf), "CSP: CAN iface FAILED (%d)", res);
        Dmesg_Write(buf);
        vTaskDelete(NULL);
        return;
    }
    Dmesg_Write("CSP: CAN interface added");

    /* Step 5: Routing */
    csp_rtable_set(CSP_DEFAULT_ROUTE, 0, can_iface, CSP_NO_VIA_ADDRESS);

    /* Step 6: Route task */
    csp_route_start_task(500, configMAX_PRIORITIES - 3);
    Dmesg_Write("CSP: routing started");

    snprintf(buf, sizeof(buf), "CSP: node %d ready", CSP_MY_ADDRESS);
    Dmesg_Write(buf);

    /* Step 7: Server (blocking) */
    csp_server_loop();
}