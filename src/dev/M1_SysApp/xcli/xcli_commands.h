#ifndef XCLI_COMMANDS_H
#define XCLI_COMMANDS_H
/*
 * xcli_commands.h
 *
 * CMD_ID range allocation example:
 *   0x00            Reserved
 *   0x01 - 0x0F    System  (help, ping, version, reboot, mem)
 *   0x10 - 0x1F    I/O     (gpio, adc, pwm, ...)
 *   0x20 - 0xEF    User    (start from XCLI_CMD_USER_BASE)
 *   0xF0 - 0xFF    Reserved
 *                                                  C.H
 */

#include <stdint.h>
#include "xcli_config.h"
#include "xcli_serial.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Wire status codes (byte [3] of every response frame)
 * =========================================================================*/
typedef uint8_t xCLI_Status_t;
#define XCLI_STATUS_OK          0x00U
#define XCLI_STATUS_UNKNOWN_CMD 0x01U
#define XCLI_STATUS_ERR         0x02U
#define XCLI_STATUS_BAD_ARG     0x03U
#define XCLI_STATUS_TIMEOUT     0x04U
#define XCLI_STATUS_BUSY        0x05U

/* =========================================================================
 * Built-in CMD_IDs
 * =========================================================================*/
/* System */
#define XCLI_CMD_HELP        0x01U
#define XCLI_CMD_PING        0x02U
#define XCLI_CMD_VERSION     0x03U
#define XCLI_CMD_REBOOT      0x04U
#define XCLI_CMD_MEM         0x05U

/* I/O */
#define XCLI_CMD_GPIO_READ   0x10U
#define XCLI_CMD_GPIO_WRITE  0x11U

/* User base */
#define XCLI_CMD_USER_BASE   0x20U
#define XCLI_CMD_TEST        0x20U
#define XCLI_CMD_COUNTER     0x21U
#define XCLI_CMD_CALC        0x22U

/* CLI tunnel */
#define XCLI_CMD_CLI         0x30U
#define XCLI_CMD_LOCAL_HELP  0x31U

/* =========================================================================
 * Platform hooks - weak stubs in xcli_commands.c
 * =========================================================================*/
void     xCLI_GetFwVersion(char *buf, uint8_t cap);
void     xCLI_GetHwVersion(char *buf, uint8_t cap);
void     xCLI_DoReboot    (uint16_t delay_ms);
uint32_t xCLI_GetFreeHeap (void);
uint32_t xCLI_GetTotalHeap(void);
uint8_t  xCLI_GpioRead    (uint8_t pin);
uint8_t  xCLI_GpioWrite   (uint8_t pin, uint8_t val);

#ifdef __cplusplus
}
#endif
#endif /* XCLI_COMMANDS_H */
