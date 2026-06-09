/*
 * xcli_commands.c 
 *
 * HOW TO ADD A COMMAND:
 *   1. Assign CMD_ID in xcli_commands.h: #define XCLI_CMD_ADC  0x23U
 *   2. Write handler:
 *        static void adc_handler(xCLI_ReqCtx_t *ctx, const char *args) {
 *            uint8_t ch = (uint8_t)xcli_token_u32(args, 1U);
 *            xCLI_Ser_PutU16(&ctx->ser, "raw", adc_read(ch));
 *        }
 *   3. Register: xCLI_RegisterCommand(&xcli, XCLI_CMD_ADC, "adc", "help", adc_handler);
 *   4. Python: rsp = host.send("adc 3", XCLI_CMD_ADC);  rsp["raw"].as_u16()
 *                                                  C.H
 */

#include "xcli_commands.h"
#include "xcli.h"
#include "xcli_serial.h"

#include <string.h>
#include <stdio.h>

#include "samv71q21b.h"

/* =========================================================================
 * Platform hook weak stubs - override in application code
 * =========================================================================*/

void xCLI_GetFwVersion(char *buf, uint8_t cap)
{
    if (buf != NULL && cap > 0U) { (void)snprintf(buf, (size_t)cap, "2.0.0"); }
}

__attribute__((weak)) void xCLI_GetHwVersion(char *buf, uint8_t cap)
{
    if (buf != NULL && cap > 0U) { buf[0] = '\0'; }
    (void)cap;
}

void xCLI_DoReboot(uint16_t delay_ms)
{
    volatile uint32_t count;

    for (count = 0; count < (uint32_t)delay_ms * 1000U; count++) {
        __NOP();
    }

    __DSB();
    __ISB();

    NVIC_SystemReset();

    while (1) {
        __NOP();
    }
}

__attribute__((weak)) uint32_t xCLI_GetFreeHeap(void)  { return 0U; }
__attribute__((weak)) uint32_t xCLI_GetTotalHeap(void) { return 0U; }

__attribute__((weak)) uint8_t xCLI_GpioRead(uint8_t pin)
{
    (void)pin; return 0U;
}

__attribute__((weak)) uint8_t xCLI_GpioWrite(uint8_t pin, uint8_t val)
{
    (void)pin; (void)val; return 0U;
}

/* =========================================================================
 * Built-in handlers
 * =========================================================================*/

#if XCLI_USE_BUILTIN_COMMANDS

/* Helper: build indexed field key like "n_0", "n_12" */
static void xcli_make_key(char *buf, char prefix, uint8_t idx)
{
    uint8_t pos = 0U;
    buf[pos++] = prefix;
    buf[pos++] = '_';
    if (idx >= 100U) { buf[pos++] = (char)('0' + (idx / 100U)); }
    if (idx >= 10U)  { buf[pos++] = (char)('0' + ((idx % 100U) / 10U)); }
    buf[pos++] = (char)('0' + (idx % 10U));
    buf[pos]   = '\0';
}

/* ---- HELP (0x01) ------------------------------------------------------- */
static void xcli_builtin_help(xCLI_ReqCtx_t *ctx, const char *args)
{
    xCLI_Instance_t *xcli = ctx->xcli;
    char  key[9];
    uint8_t i;
    (void)args;

    xCLI_Ser_PutU8(&ctx->ser, "count", xcli->cmd_count);
    for (i = 0U; i < xcli->cmd_count; i++) {
        xcli_make_key(key, 'i', i);  xCLI_Ser_PutU8 (&ctx->ser, key, xcli->cmds[i].cmd_id);
        xcli_make_key(key, 'n', i);  xCLI_Ser_PutStr (&ctx->ser, key, xcli->cmds[i].name);
        xcli_make_key(key, 'd', i);  xCLI_Ser_PutStr (&ctx->ser, key, xcli->cmds[i].help);
        if (xCLI_Ser_Error(&ctx->ser)) { break; }
    }
}

/* ---- PING (0x02) ------------------------------------------------------- */
static void xcli_builtin_ping(xCLI_ReqCtx_t *ctx, const char *args)
{
    uint32_t ts = xcli_token_u32(args, 1U);
    xCLI_Ser_PutU32(&ctx->ser, "echo", ts);
}

/* ---- VERSION (0x03) ---------------------------------------------------- */
static void xcli_builtin_version(xCLI_ReqCtx_t *ctx, const char *args)
{
    char fw[XCLI_MAX_CMD_HELP];
    char hw[XCLI_MAX_CMD_HELP];
    (void)args;
    xCLI_GetFwVersion(fw, (uint8_t)sizeof(fw));
    xCLI_GetHwVersion(hw, (uint8_t)sizeof(hw));
    xCLI_Ser_PutStr(&ctx->ser, "fw", fw);
    xCLI_Ser_PutStr(&ctx->ser, "hw", hw);
}

/* ---- REBOOT (0x04) ----------------------------------------------------- */
static void xcli_builtin_reboot(xCLI_ReqCtx_t *ctx, const char *args)
{
    uint16_t delay_ms = (uint16_t)xcli_token_u32(args, 1U);
    xCLI_Ser_PutBool(&ctx->ser, "ack", 1U);
    xCLI_DoReboot(delay_ms);
}

/* ---- MEM (0x05) -------------------------------------------------------- */
static void xcli_builtin_mem(xCLI_ReqCtx_t *ctx, const char *args)
{
    (void)args;
    xCLI_Ser_PutU32(&ctx->ser, "free",  xCLI_GetFreeHeap());
    xCLI_Ser_PutU32(&ctx->ser, "total", xCLI_GetTotalHeap());
}

/* ---- GPIO (0x10) ------------------------------------------------------- */
static void xcli_builtin_gpio(xCLI_ReqCtx_t *ctx, const char *args)
{
    const char *subcmd = embeddedCliGetToken(args, 1U);
    uint8_t     pin;

    if (subcmd == NULL) { ctx->status = XCLI_STATUS_BAD_ARG; return; }
    pin = (uint8_t)xcli_token_u32(args, 2U);

    if (subcmd[0] == 'r' || subcmd[0] == 'R') {
        xCLI_Ser_PutU8(&ctx->ser, "pin",   pin);
        xCLI_Ser_PutU8(&ctx->ser, "state", xCLI_GpioRead(pin));
    } else if (subcmd[0] == 'w' || subcmd[0] == 'W') {
        uint8_t val = (uint8_t)xcli_token_u32(args, 3U);
        xCLI_Ser_PutU8  (&ctx->ser, "pin",   pin);
        xCLI_Ser_PutU8  (&ctx->ser, "state", xCLI_GpioWrite(pin, val));
        xCLI_Ser_PutBool(&ctx->ser, "ok",    1U);
    } else {
        ctx->status = XCLI_STATUS_BAD_ARG;
    }
}

/* ---- TEST (0x20) ------------------------------------------------------- */
static uint8_t test_seq_v = 0U;
static void test_handler(xCLI_ReqCtx_t *ctx, const char *args)
{
    uint8_t ch = (uint8_t)xcli_token_u32(args, 1U);
    if (ch > 7U) { ctx->status = XCLI_STATUS_BAD_ARG; return; }
    xCLI_Ser_PutU8(&ctx->ser, "test", ch);
    xCLI_Ser_PutU8(&ctx->ser, "data", test_seq_v++);
}

/* ---- COUNTER (0x21) ---------------------------------------------------- */
static uint32_t s_counter_value = 0U;
static uint32_t s_counter_calls = 0U;
static uint8_t  s_counter_max   = 0U;

static void counter_handler(xCLI_ReqCtx_t *ctx, const char *args)
{
    const char *sub = embeddedCliGetToken(args, 1U);
    uint8_t did_reset = 0U;
    s_counter_calls++;

    if (sub != NULL && (sub[0] == 'r' || sub[0] == 'R')) {
        s_counter_value = 0U; s_counter_max = 0U; did_reset = 1U;
    } else {
        uint8_t step = (sub != NULL) ? (uint8_t)xcli_token_u32(args, 1U) : 1U;
        if (step == 0U) { step = 1U; }
        s_counter_value += step;
        if (step > s_counter_max) { s_counter_max = step; }
    }

    xCLI_Ser_PutU32 (&ctx->ser, "value",     s_counter_value);
    xCLI_Ser_PutU32 (&ctx->ser, "calls",     s_counter_calls);
    xCLI_Ser_PutU8  (&ctx->ser, "max_step",  s_counter_max);
    xCLI_Ser_PutBool(&ctx->ser, "was_reset", did_reset);
}

/* ---- CALC (0x22) ------------------------------------------------------- */
static void calc_handler(xCLI_ReqCtx_t *ctx, const char *args)
{
    const char *tok_op = embeddedCliGetToken(args, 2U);
    uint32_t a, b, result = 0U;
    char     op;
    char     op_str[2];

    if (embeddedCliGetToken(args, 1U) == NULL ||
        tok_op == NULL ||
        embeddedCliGetToken(args, 3U) == NULL) {
        ctx->status = XCLI_STATUS_BAD_ARG; return;
    }

    a  = xcli_token_u32(args, 1U);
    op = tok_op[0];
    b  = xcli_token_u32(args, 3U);
    op_str[0] = op; op_str[1] = '\0';

    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': case '%':
            if (b == 0U) { ctx->status = XCLI_STATUS_BAD_ARG; return; }
            result = (op == '/') ? (a / b) : (a % b);
            break;
        default: ctx->status = XCLI_STATUS_BAD_ARG; return;
    }

    xCLI_Ser_PutU32 (&ctx->ser, "result", result);
    xCLI_Ser_PutStr (&ctx->ser, "op",     op_str);
    xCLI_Ser_PutU32 (&ctx->ser, "a",      a);
    xCLI_Ser_PutU32 (&ctx->ser, "b",      b);
    xCLI_Ser_PutBool(&ctx->ser, "ok",     1U);
}

/* ---- Registration ------------------------------------------------------ */
void xCLI_RegisterBuiltins(xCLI_Instance_t *xcli)
{
    if (xcli == NULL) { return; }
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_HELP,
        "help",    "List all registered commands",          xcli_builtin_help);
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_PING,
        "ping",    "Echo a 32-bit timestamp",               xcli_builtin_ping);
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_VERSION,
        "version", "Report firmware/hardware version",      xcli_builtin_version);
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_REBOOT,
        "reboot",  "Reboot device [delay_ms]",              xcli_builtin_reboot);
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_MEM,
        "mem",     "Report heap statistics",                xcli_builtin_mem);
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_GPIO_READ,
        "gpio",    "gpio read <pin> | gpio write <pin> <v>",xcli_builtin_gpio);
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_TEST,
        "test",    "Read channel 0-7",                      test_handler);
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_COUNTER,
        "counter", "Increment counter [n|reset]",           counter_handler);
    (void)xCLI_RegisterCommand(xcli, XCLI_CMD_CALC,
        "calc",    "Arithmetic: calc <a> <+|-|*|/|%> <b>", calc_handler);
}

#endif /* XCLI_USE_BUILTIN_COMMANDS */
