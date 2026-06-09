/*
 * can_mcan.c v4.0
 *
 * Fix history:
 *  v1: xSemaphoreGiveFromISR --> Hang when wrong NVIC priority
 *  v2: volatile flag + while(MessageReceiveFifo) -> infinite loop -> flood
 *  v3: Create limit DRAIN_LIMIT=100 -> works but it hacky
 *  v4: F0FL-based drain + delay in task for yields
 */

#include "csp/drivers/can_mcan.h"

#include <string.h>
#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/interfaces/csp_if_can.h>

#include "peripheral/mcan/plib_mcan1.h"
#include "os.h"
#include "task.h"

/* ================================================================
 *  Configuration
 * ================================================================ */
#define MCAN_RX_TASK_STACK_SIZE   (configMINIMAL_STACK_SIZE * 10)
#define MCAN_RX_TASK_PRIORITY     (configMAX_PRIORITIES - 2)
#define MCAN_TX_TIMEOUT_MS        1000

#define MCAN_RX_POLL_DELAY_MS     1

/* ================================================================
 *  Driver context
 * ================================================================ */
typedef struct {
    char                     name[CSP_IFLIST_NAME_MAX + 1];
    csp_iface_t              iface;
    csp_can_interface_data_t ifdata;
    TaskHandle_t             rx_task;
    volatile bool            rx_pending;
    volatile bool            running;
} mcan_context_t;

static uint8_t __attribute__((aligned(4))) mcan1MsgRAM[MCAN1_MESSAGE_RAM_CONFIG_SIZE];
static mcan_context_t mcan_ctx;

volatile uint32_t g_mcan_isr_count   = 0;
volatile uint32_t g_mcan_rx_accepted = 0;  /* Extended frames -> CSP */
volatile uint32_t g_mcan_rx_dropped  = 0;  /* Standard/ESI/RTR -> discarded */

/* ================================================================
 *  ISR callbacks
 * ================================================================ */
static void mcan_rx_fifo0_callback(uint8_t numberOfMessage, uintptr_t context)
{
    (void)numberOfMessage;
    (void)context;
    g_mcan_isr_count++;
    mcan_ctx.rx_pending = true;
}

static void mcan_rx_fifo1_callback(uint8_t numberOfMessage, uintptr_t context)
{
    (void)numberOfMessage;
    (void)context;
    g_mcan_isr_count++;
    mcan_ctx.rx_pending = true;
}

static void mcan_error_callback(uint32_t interruptStatus, uintptr_t context)
{
    (void)interruptStatus;
    (void)context;
}

/* ================================================================
 *  TX
 * ================================================================ */
static int csp_can_tx_frame(void *driver_data, uint32_t id,
                            const uint8_t *data, uint8_t dlc)
{
    (void)driver_data;
    if (dlc > 8) return CSP_ERR_INVAL;

    MCAN_TX_BUFFER txBuffer;
    memset(&txBuffer, 0, sizeof(txBuffer));
    txBuffer.id  = id & 0x1FFFFFFFU;
    txBuffer.xtd = 1;
    txBuffer.dlc = dlc;
    txBuffer.fdf = 0;
    txBuffer.brs = 0;
    if (data && dlc > 0) memcpy(txBuffer.data, data, dlc);

    uint32_t elapsed_ms = 0;
    while (MCAN1_TxFifoFreeLevelGet() == 0) {
        if (elapsed_ms >= MCAN_TX_TIMEOUT_MS) {
            csp_log_warn("MCAN TX timeout");
            return CSP_ERR_TX;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
        elapsed_ms += 5;
    }

    if (!MCAN1_MessageTransmitFifo(1, &txBuffer)) {
        csp_log_warn("MCAN TX failed");
        return CSP_ERR_TX;
    }
    return CSP_ERR_NONE;
}

/* ================================================================
 *  Helper: Create drain FIFO with fill level :)
 * ================================================================ */
static void drain_fifo(mcan_context_t *ctx, MCAN_RX_FIFO_NUM fifo_num,
                       uint32_t f_fl_reg, uint32_t f_fl_msk, uint32_t f_fl_pos)
{
    MCAN_RX_BUFFER rxBuffer;

    uint32_t to_read = (f_fl_reg & f_fl_msk) >> f_fl_pos;

    while (to_read > 0 && MCAN1_MessageReceiveFifo(fifo_num, 1, &rxBuffer)) {
        to_read--;

        if (rxBuffer.xtd == 0 || rxBuffer.esi || rxBuffer.rtr) {
            /*
             * Standard frame, error frame, or RTR:
             * Frame already popped FIFO hardware.
             * -> Discard
             */
            g_mcan_rx_dropped++;
        } else {
            /* Extended frame --> CSP */
            uint32_t can_id = rxBuffer.id & 0x1FFFFFFFU;
            g_mcan_rx_accepted++;
            csp_can_rx(&ctx->iface, can_id, rxBuffer.data, rxBuffer.dlc, NULL);
        }
    }
}

/* ================================================================
 *  RX task
 * ================================================================ */
static void mcan_rx_task(void *param)
{
    mcan_context_t *ctx = (mcan_context_t *)param;

    while (ctx->running) {

        if (ctx->rx_pending) {
            ctx->rx_pending = false;

            drain_fifo(ctx, MCAN_RX_FIFO_0,
                       MCAN1_REGS->MCAN_RXF0S,
                       MCAN_RXF0S_F0FL_Msk, MCAN_RXF0S_F0FL_Pos);

            drain_fifo(ctx, MCAN_RX_FIFO_1,
                       MCAN1_REGS->MCAN_RXF1S,
                       MCAN_RXF1S_F1FL_Msk, MCAN_RXF1S_F1FL_Pos);
        }
        vTaskDelay(pdMS_TO_TICKS(MCAN_RX_POLL_DELAY_MS));
    }

    osThreadExit();
}

/* ================================================================
 *  Public API
 * ================================================================ */
int csp_can_mcan_open_and_add_interface(const char *ifname, bool promisc,
                                        csp_iface_t **return_iface)
{
    if (ifname == NULL) ifname = CSP_IF_CAN_DEFAULT_NAME;

    memset(&mcan_ctx, 0, sizeof(mcan_ctx));
    strncpy(mcan_ctx.name, ifname, sizeof(mcan_ctx.name) - 1);
    mcan_ctx.iface.name           = mcan_ctx.name;
    mcan_ctx.iface.interface_data = &mcan_ctx.ifdata;
    mcan_ctx.iface.driver_data    = &mcan_ctx;
    mcan_ctx.ifdata.tx_func       = csp_can_tx_frame;
    mcan_ctx.running              = true;
    mcan_ctx.rx_pending           = false;

    MCAN1_MessageRAMConfigSet(mcan1MsgRAM);

    MCAN1_RxFifoCallbackRegister(MCAN_RX_FIFO_0, mcan_rx_fifo0_callback, 0);
    MCAN1_RxFifoCallbackRegister(MCAN_RX_FIFO_1, mcan_rx_fifo1_callback, 0);
    MCAN1_CallbackRegister(mcan_error_callback, 0);

    int res = csp_can_add_interface(&mcan_ctx.iface);
    if (res != CSP_ERR_NONE) {
        csp_log_error("MCAN: csp_can_add_interface() failed: %d", res);
        return res;
    }

    BaseType_t xRet = xTaskCreate(
        mcan_rx_task, "CSP_CAN_RX",
        MCAN_RX_TASK_STACK_SIZE,
        &mcan_ctx,
        MCAN_RX_TASK_PRIORITY,
        &mcan_ctx.rx_task
    );

    if (xRet != pdPASS) {
        csp_log_error("MCAN: xTaskCreate() failed");
        return CSP_ERR_NOMEM;
    }

    if (return_iface) *return_iface = &mcan_ctx.iface;

    csp_log_info("MCAN: [%s] ready", ifname);
    return CSP_ERR_NONE;
}

int csp_can_mcan_stop(csp_iface_t *iface)
{
    mcan_context_t *ctx = iface->driver_data;
    ctx->running = false;
    vTaskDelay(pdMS_TO_TICKS(50));
    if (ctx->rx_task) {
        vTaskDelete(ctx->rx_task);
        ctx->rx_task = NULL;
    }
    return CSP_ERR_NONE;
}