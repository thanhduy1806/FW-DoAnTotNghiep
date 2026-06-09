/*
 * cli_setup.c
 *
 *  Created on: Feb 27, 2025
 *      Author: CAO HIEU
 */

#include "cli_setup.h"
#include "M2_BSP/UART/uart_irq.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"

#define USE_RTOS

#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#define UART2_TX_YIELD_THRESHOLD   127
static uint16_t s_uart2_tx_count = 0;
#endif

#define UART2_CLI_BUFFER_SIZE   3072
#define NET_CLI_BUFFER_SIZE     3072
#define RGOSH_CLI_BUFFER_SIZE   2048 

static CLI_UINT uart2_cliStaticBuffer[BYTES_TO_CLI_UINTS(UART2_CLI_BUFFER_SIZE)];
static CLI_UINT net_cliStaticBuffer  [BYTES_TO_CLI_UINTS(NET_CLI_BUFFER_SIZE)];
static CLI_UINT rgosh_cliStaticBuffer[BYTES_TO_CLI_UINTS(RGOSH_CLI_BUFFER_SIZE)];

/* ================================================================
 *  Instance pointers
 * ================================================================ */
static EmbeddedCli *cli_uart2;
static EmbeddedCli *cli_net;
static EmbeddedCli *cli_rgosh;

static _Bool cliIsReady = false;

/* ================================================================
 *  writeChar callbacks
 * ================================================================ */
static CliWriteByte_t pf_Net_WriteChar = NULL;

static void writeCharToCli_UART2(EmbeddedCli *cli, char c)
{
    (void)cli; 

    UART2_WriteByte((uint8_t)c);

#ifdef USE_RTOS 

    s_uart2_tx_count++;

    if (s_uart2_tx_count >= UART2_TX_YIELD_THRESHOLD)
    {
        s_uart2_tx_count = 0;

        if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
        {
            vTaskDelay(10);  
        }
    }

#endif
}

static void writeCharToCli_Net(EmbeddedCli *cli, char c) {
    (void)cli;
    if (pf_Net_WriteChar != NULL)
        pf_Net_WriteChar(c);
}

static bool   s_rgosh_capture = false;
static char  *s_rgosh_out_buf = NULL;
static size_t s_rgosh_out_max = 0;
static size_t s_rgosh_out_len = 0;

static void writeCharToCli_RGOSH(EmbeddedCli *cli, char c) {
    (void)cli;
    if (!s_rgosh_capture) return;
    if (s_rgosh_out_buf && s_rgosh_out_len < s_rgosh_out_max - 1)
        s_rgosh_out_buf[s_rgosh_out_len++] = c;
}

/* ================================================================
 *  SystemCLI_Init
 *  Call once: after peripheral init, before RTOS scheduler.
 * ================================================================ */
Std_ReturnType SystemCLI_Init(void)
{
    EmbeddedCliConfig *cfg;

    /* ---- 1. UART2 CLI ---- */
    cfg = embeddedCliDefaultConfig();
    cfg->cliBuffer          = uart2_cliStaticBuffer;
    cfg->cliBufferSize      = UART2_CLI_BUFFER_SIZE;
    cfg->rxBufferSize       = CLI_RX_BUFFER_SIZE;
    cfg->cmdBufferSize      = CLI_CMD_BUFFER_SIZE;
    cfg->historyBufferSize  = CLI_HISTORY_SIZE;
    cfg->maxBindingCount    = CLI_MAX_BINDING_COUNT;
    cfg->enableAutoComplete = CLI_AUTO_COMPLETE;
    cfg->invitation         = CLI_INITATION_UART2;
    cfg->staticBindings     = getCliStaticBindings();
    cfg->staticBindingCount = getCliStaticBindingCount();

    cli_uart2 = embeddedCliNew(cfg);
    if (cli_uart2 == NULL) return ERROR_FAIL;
    cli_uart2->writeChar = writeCharToCli_UART2;

    /* ---- 2. Net CLI ---- */
    cfg = embeddedCliDefaultConfig();
    cfg->cliBuffer          = net_cliStaticBuffer;
    cfg->cliBufferSize      = NET_CLI_BUFFER_SIZE;
    cfg->rxBufferSize       = CLI_RX_BUFFER_SIZE;
    cfg->cmdBufferSize      = CLI_CMD_BUFFER_SIZE;
    cfg->historyBufferSize  = CLI_HISTORY_SIZE;
    cfg->maxBindingCount    = CLI_MAX_BINDING_COUNT;
    cfg->enableAutoComplete = 0;
    cfg->invitation         = CLI_INITATION_NET;
    cfg->staticBindings     = getCliStaticBindings();
    cfg->staticBindingCount = getCliStaticBindingCount();

    cli_net = embeddedCliNew(cfg);
    if (cli_net == NULL) return ERROR_FAIL;
    cli_net->writeChar = writeCharToCli_Net;

    /* ---- 3. RGOSH CLI ---- */
    cfg = embeddedCliDefaultConfig();
    cfg->cliBuffer          = rgosh_cliStaticBuffer;
    cfg->cliBufferSize      = RGOSH_CLI_BUFFER_SIZE;
    cfg->rxBufferSize       = 128;
    cfg->cmdBufferSize      = 128;
    cfg->historyBufferSize  = 0;   
    cfg->maxBindingCount    = CLI_MAX_BINDING_COUNT;
    cfg->enableAutoComplete = 0;
    cfg->invitation         = "";     
    cfg->staticBindings     = getCliStaticBindings();
    cfg->staticBindingCount = getCliStaticBindingCount();

    cli_rgosh = embeddedCliNew(cfg);
    if (cli_rgosh == NULL) return ERROR_FAIL;
    cli_rgosh->writeChar = writeCharToCli_RGOSH;

    cliIsReady = true;
    return ERROR_OK;
}

/* ================================================================
 *  Getters
 * ================================================================ */
EmbeddedCli *get_UART2_CliPointer(void) { return cli_uart2; }
EmbeddedCli *get_Net_CliPointer(void)   { return cli_net;   }
EmbeddedCli *get_RGOSH_CliPointer(void) { return cli_rgosh; }

void Cli_Net_Register_Tx(CliWriteByte_t tx_cb) {
    pf_Net_WriteChar = tx_cb;
}

void Cli_RGOSH_CaptureStart(char *buf, size_t buf_max)
{
    s_rgosh_out_buf = buf;
    s_rgosh_out_max = buf_max;
    s_rgosh_out_len = 0;
    if (buf && buf_max > 0) buf[0] = '\0';
    s_rgosh_capture = true;
}

void Cli_RGOSH_CaptureStop(size_t *out_len)
{
    s_rgosh_capture = false;
    if (s_rgosh_out_buf && s_rgosh_out_len < s_rgosh_out_max)
        s_rgosh_out_buf[s_rgosh_out_len] = '\0';
    if (out_len) *out_len = s_rgosh_out_len;
}