/************************************************
 *  @file     : app_cli.c
 *  @date     : January 2026
 *  @author   : CAO HIEU
 *  @version  : 1.0.0
 *-----------------------------------------------
 *  Description :
 *    [-]
 ************************************************/

#include "app_cli.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "M2_BSP/UART/uart_irq.h"
#include "M5_Utils/DateTime/date_time.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Auth/simple_shield.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_temperature.h"

#define WIZ_BUF_MAX 64

typedef enum {
    CLI_NOMAL = 0,
    CLI_WIZARD = 1,
} e_cli_mode_t;

static e_cli_mode_t cli_mode_state = CLI_NOMAL;
static uint8_t wizard_total = 0;
static uint8_t wizard_remain = 0;
static char wiz_buf[WIZ_BUF_MAX];
static uint8_t wiz_idx = 0;

void app_cli_wizard_start(uint8_t total_step) {
    cli_mode_state = CLI_WIZARD;
    wizard_total = total_step;
    wizard_remain = total_step;
}

ShieldInstance_t auth_uart;

static void writeChar_auth_UART2(char c) {
    UART2_WriteByte(c);
}

static void CLI_Handle_Thread(void) {
    if (cli_mode_state == CLI_WIZARD)
        return;
    ShieldAuthState_t auth_state;
    auth_state = Shield_GetState(&auth_uart);
    if (auth_state == AUTH_ADMIN || auth_state == AUTH_USER) {
        if (auth_uart.initreset == 1) {
            embeddedCliPrint(get_UART2_CliPointer(), "");
            auth_uart.initreset = 0;
        }
        embeddedCliProcess(get_UART2_CliPointer());
    }
}

static void UART2_RX_Thread(void) {
    int c;
    while ((c = UART2_ReadByte()) != -1) {
        // Mode NORMAL: CLI
        // ShieldAuthState_t auth_state = Shield_GetState(&auth_uart);
        ShieldAuthState_t auth_state = AUTH_ADMIN;
        if (auth_state == AUTH_ADMIN || auth_state == AUTH_USER) {
            Shield_ResetTimer(&auth_uart);

            /* OPTION 1: Begin*/
            // embeddedCliReceiveChar(get_UART2_CliPointer(), (char) c);
            // embeddedCliProcess(get_UART2_CliPointer());
            /* OPTION 1: Stop*/

            /* OPTION 2: Begin*/
            if (cli_mode_state == CLI_WIZARD) {
                UART2_WriteByte((char) c);
                if (c == '\r' || c == '\n') {
                    wiz_buf[wiz_idx] = '\0';
                    wiz_idx = 0;
                    uint8_t step = wizard_total - wizard_remain;
                    // ===== STEP 0: CONFIRM =====
                    if (step == GET_CONFIRM) {
                        if (wiz_buf[0] == 'Y' || wiz_buf[0] == 'y') {
                            TempProfile_InputHandler(get_UART2_CliPointer(), wiz_buf, step);
                            if (wizard_remain > 0) wizard_remain--;
                        } else if (wiz_buf[0] == 'N' || wiz_buf[0] == 'n') {
                            cli_mode_state = CLI_NOMAL;
                            UART2_SendString("\r\nCancelled\r\n");
                        } else {
                            UART2_SendString("\r\nPlease enter Y or N: ");
                        }
                    } else if (step == GET_STEP) {
                        TempProfile_InputHandler(get_UART2_CliPointer(), wiz_buf, step);
                        if (TempProfile_CheckRemainStep() == 0)
                            if (wizard_remain > 0)
                                wizard_remain--;
                    }                        // ===== STEP SAVE =====
                    else if (step == GET_SAVE) {
                        if (wiz_buf[0] == 'Y' || wiz_buf[0] == 'y') {
                            TempProfile_InputHandler(get_UART2_CliPointer(), wiz_buf, step);
                            cli_mode_state = CLI_NOMAL;
                            UART2_SendString("\r\nSaved");
                        } else if (wiz_buf[0] == 'N' || wiz_buf[0] == 'n') {
                            cli_mode_state = CLI_NOMAL;
                            UART2_SendString("\r\nDiscarded\r\n");
                        } else {
                            UART2_SendString("\r\nPlease enter Y or N: ");
                        }
                    }                        // ===== OTHER STEPS =====
                    else {
                        TempProfile_InputHandler(get_UART2_CliPointer(), wiz_buf, step);
                        if (wizard_remain > 0) wizard_remain--;
                    }
                } else {
                    if (wiz_idx < WIZ_BUF_MAX - 1) {
                        wiz_buf[wiz_idx++] = (char) c;
                    }
                }
            } else {
                // ===== Normal CLI =====
                embeddedCliReceiveChar(get_UART2_CliPointer(), (char) c);
                embeddedCliProcess(get_UART2_CliPointer());
            }
            /* OPTION 2: Stop*/

        } else {
            Shield_ReceiveChar(&auth_uart, (char) c);
        }
    }
}

/*-----------------------------------------------------------------*/
const osThreadAttr_t cli_attr = {
    .name = "CLI",
    .stack_size = configMINIMAL_STACK_SIZE * 10,
    .priority = configMAX_PRIORITIES - 3
};

void App_CLITask(void *param) {
    Shield_Init(&auth_uart, writeChar_auth_UART2);

    for (;;) {

        UART2_RX_Thread();
        CLI_Handle_Thread();
        osThreadFeed();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}