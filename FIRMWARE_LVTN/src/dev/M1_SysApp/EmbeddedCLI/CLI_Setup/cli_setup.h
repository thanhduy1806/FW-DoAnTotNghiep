/*
 * cli_setup.h
 *
 *  Created on: Feb 27, 2025
 *      Author: CAO HIEU
 */

#ifndef M2_SYSTEM_CLI_TERMINAL_CLI_SETUP_CLI_SETUP_H_
#define M2_SYSTEM_CLI_TERMINAL_CLI_SETUP_CLI_SETUP_H_
#include "../CLI_Src/embedded_cli.h"
#include "dev/M5_Utils/Define/define.h"
#include "stddef.h"
// Definitions for CLI sizes
#define CLI_RX_BUFFER_SIZE 		16
#define CLI_CMD_BUFFER_SIZE 	64
#define CLI_HISTORY_SIZE 		128
#define CLI_MAX_BINDING_COUNT 	32
#define CLI_AUTO_COMPLETE 		1
#define CLI_INITATION_UART2		"DEBUG@MCU:~ $ "
#define CLI_INITATION_NET		"USB@MCU:~ $ "

/**
 * Char for char is needed for instant char echo, so size 1
 */
//#define UART_RX_BUFF_SIZE 1
typedef void (*CliWriteByte_t)(char c);

/**
 * Function to setup the configuration settings for the CLI,
 * based on the definitions from this header file
 */
Std_ReturnType SystemCLI_Init();

/**
 * Getter function, to keep only one instance of the EmbeddedCli pointer in this file.
 * @return
 */
EmbeddedCli *get_UART2_CliPointer(void);

EmbeddedCli *get_Net_CliPointer(void);

EmbeddedCli *get_RGOSH_CliPointer(void);

void Cli_Net_Register_Tx(CliWriteByte_t tx_cb);

void Cli_RGOSH_CaptureStart(char *buf, size_t buf_max);
void Cli_RGOSH_CaptureStop(size_t *out_len);

#endif /* M2_SYSTEM_CLI_TERMINAL_CLI_SETUP_CLI_SETUP_H_ */
