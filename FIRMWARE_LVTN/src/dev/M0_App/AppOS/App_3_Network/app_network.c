/************************************************
 *  @file     : app_network.c
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "app_network.h"

#include "tusb.h"
#include "tusb_config.h"
// --- LWIP HEADERS ---
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "board_api.h"

#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Auth/simple_shield.h"

// sys_prot_t sys_arch_protect(void) {
//     taskENTER_CRITICAL();
// }

// void sys_arch_unprotect(sys_prot_t pval) {
//     (void)pval;
//     taskEXIT_CRITICAL();
// }

// uint32_t sys_now(void) {
//     return board_millis();
// }

#define INIT_IP4(a, b, c, d) { PP_HTONL(LWIP_MAKEU32(a, b, c, d)) }

static struct netif netif_data;
uint8_t tud_network_mac_address[6] = {0x02, 0x02, 0x84, 0x6A, 0x96, 0x00};

static const ip4_addr_t ipaddr  = INIT_IP4(192, 168, 6, 8); 
static const ip4_addr_t netmask = INIT_IP4(255, 255, 255, 0);
static const ip4_addr_t gateway = INIT_IP4(0, 0, 0, 0);     

// --- TCP SERVER VARIABLES ---
#define TCP_PORT 2323

/* -------------------------------------------------------------------------- */
/* CLI INTERFACE                                */
/* -------------------------------------------------------------------------- */
static struct tcp_pcb *current_cli_pcb = NULL;
static ShieldInstance_t auth_net;

static void Network_CLI_Tx_Wrapper(char c) {
    if (current_cli_pcb != NULL) {
        tcp_write(current_cli_pcb, &c, 1, TCP_WRITE_FLAG_COPY);
    }
}

/* ------------------ LWIP Stack ---------------*/
static void tcp_server_send(struct tcp_pcb *tpcb, const char *msg) {
    tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb); 
}

static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    LWIP_UNUSED_ARG(arg);

    if (p == NULL) {
        tcp_close(tpcb);
        current_cli_pcb = NULL;
        Cli_Net_Register_Tx(NULL); 
        DEBUG_SendString("TCP: Connection closed by client\r\n");
        return ERR_OK;
    }

    if (err != ERR_OK) {
        pbuf_free(p);
        return err;
    }

    tcp_recved(tpcb, p->tot_len);

    struct pbuf *q;
    EmbeddedCli *net_cli = get_Net_CliPointer();

    if (net_cli != NULL) {
        for (q = p; q != NULL; q = q->next) {
            char *data = (char *)q->payload;
            for (int i = 0; i < q->len; i++) {
                char c = data[i];
                if (c == '\n') continue;
                if ((unsigned char)c == 0xFF) continue;

                // --- LOGIC AUTHENTICATION ---
                ShieldAuthState_t auth_state = Shield_GetState(&auth_net);
                
                if(auth_state == AUTH_ADMIN || auth_state == AUTH_USER) {
                    Shield_ResetTimer(&auth_net);
                    embeddedCliReceiveChar(net_cli, c);
                } else {
                    Shield_ReceiveChar(&auth_net, c);
                }
            }
        }
    }

    pbuf_free(p);
    return ERR_OK;
}

static void tcp_server_err(void *arg, err_t err) {
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    
    current_cli_pcb = NULL;
    Cli_Net_Register_Tx(NULL);
    Shield_Init(&auth_net, Network_CLI_Tx_Wrapper);
    DEBUG_SendString("TCP: Connection aborted/error\r\n");
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

if (current_cli_pcb != NULL) {
        DEBUG_SendString("TCP: Kicking old client for new connection...\r\n");
        
        tcp_arg(current_cli_pcb, NULL);
        tcp_recv(current_cli_pcb, NULL);
        tcp_err(current_cli_pcb, NULL);
        tcp_poll(current_cli_pcb, NULL, 0);
        
        tcp_abort(current_cli_pcb);

        current_cli_pcb = NULL;
    }

    DEBUG_SendString("TCP: New connection accepted\r\n");
    current_cli_pcb = newpcb;

    // 1. Register Callbacks
    tcp_recv(newpcb, tcp_server_recv);
    tcp_err(newpcb, tcp_server_err);
    // tcp_poll(newpcb, tcp_server_poll, 4); // Optional if need periodic callback

    // 2. Register TX Function to CLI Setup
    // Now, when CLI calls writeChar, it goes to Network_CLI_Tx_Wrapper -> tcp_write
    Cli_Net_Register_Tx(Network_CLI_Tx_Wrapper);

    Shield_Init(&auth_net, Network_CLI_Tx_Wrapper);
    // 3. Send Welcome Message (Directly via TCP first)
    tcp_write(newpcb, "--- Connected to MCU CLI ---\r\n", 33, TCP_WRITE_FLAG_COPY);
    
    // 4. Force CLI to reprint prompt
    EmbeddedCli *net_cli = get_Net_CliPointer();
    if(net_cli) {
        // Use a trick: Send a null char or specific command to refresh, 
        // or just print the prompt manually if exposed.
        // For now, simple newline usually triggers prompt repaint in next process cycle
        embeddedCliReceiveChar(net_cli, '\n'); 
    }
    
    tcp_output(newpcb); // Flush welcome message
    return ERR_OK;
}

void tcp_server_init(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (pcb != NULL) {
        err_t err = tcp_bind(pcb, IP_ADDR_ANY, TCP_PORT);
        
        if (err == ERR_OK) {
            pcb = tcp_listen(pcb);
            tcp_accept(pcb, tcp_server_accept);
            DEBUG_SendString("TCP Server listening on port 2323\r\n");
        } else {
            DEBUG_SendString("TCP Error: Bind failed \r\n");
            memp_free(MEMP_TCP_PCB, pcb);
        }
    } else {
        DEBUG_SendString("TCP Error: Could not create PCB\r\n");
    }
}

/* -------------------------------------------------------------------------- */
/* NETWORK DRIVER                              */
/* -------------------------------------------------------------------------- */
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p) {
  (void) netif;
  for (;;) {
    if (!tud_ready()) return ERR_USE;
    if (tud_network_can_xmit(p->tot_len)) {
      tud_network_xmit(p, 0);
      return ERR_OK;
    }
    tud_task();
  }
}

static err_t ip4_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr) {
  return etharp_output(netif, p, addr);
}

static err_t netif_init_cb(struct netif *netif) {
  netif->mtu = CFG_TUD_NET_MTU;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
  netif->name[0] = 'E'; netif->name[1] = 'X';
  netif->linkoutput = linkoutput_fn;
  netif->output = ip4_output_fn;
  return ERR_OK;
}

static void usbnet_netif_link_callback(struct netif *netif) {
    bool link_up = netif_is_link_up(netif);
    tud_network_link_state(0, link_up);
    if(link_up) DEBUG_SendString("NETIF: Link UP\r\n");
    else        DEBUG_SendString("NETIF: Link DOWN\r\n");
}

static void init_lwip(void) {
  struct netif *netif = &netif_data;
  lwip_init();
  netif->hwaddr_len = sizeof(tud_network_mac_address);
  memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));
  netif->hwaddr[5] ^= 0x01; 

  netif = netif_add(netif, &ipaddr, &netmask, &gateway, NULL, netif_init_cb, ethernet_input);
  netif_set_default(netif);
  netif_set_link_callback(netif, usbnet_netif_link_callback);
  
  netif_set_link_up(netif); 
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size) {
  if (size) {
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    if (p) {
      pbuf_take(p, src, size);
      if (netif_data.input(p, &netif_data) != ERR_OK) pbuf_free(p);
    }
    tud_network_recv_renew();
  }
  return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg) {
  struct pbuf *p = (struct pbuf *) ref;
  return pbuf_copy_partial(p, dst, p->tot_len, 0);
}

void tud_network_init_cb(void) {
}

const osThreadAttr_t network_attr = {
        .name = "Network_Task",
        .stack_size = 8192,
        .priority = configMAX_PRIORITIES - 3
};

void App_NetworkTask(void *param) {
    init_lwip();
    while (!netif_is_up(&netif_data));
    
    tcp_server_init();
    Shield_Init(&auth_net, Network_CLI_Tx_Wrapper);

    DEBUG_SendString("System Started. IP: 192.168.6.8, Port: 2323\r\n");
    EmbeddedCli *net_cli = get_Net_CliPointer();

    for(;;) {
        sys_check_timeouts();
        if (net_cli != NULL) {
            ShieldAuthState_t auth_state = Shield_GetState(&auth_net);
            
            if(auth_state == AUTH_ADMIN || auth_state == AUTH_USER)
            {
                if(auth_net.initreset == 1) {
                    embeddedCliPrint(net_cli, ""); 
                    auth_net.initreset = 0;
                }
                
                embeddedCliProcess(net_cli);
            }
        }
        if (current_cli_pcb != NULL) {
            tcp_output(current_cli_pcb);
        }
        osThreadFeed();
        osDelay(pdMS_TO_TICKS(5));
    }
}