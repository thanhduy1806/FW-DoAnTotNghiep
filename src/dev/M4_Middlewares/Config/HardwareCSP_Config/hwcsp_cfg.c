#include "hwcsp_cfg.h"
#include <csp/arch/csp_system.h>
#include <csp/csp_debug.h>
#include <stdio.h>
#include <csp/csp.h>
#include "M2_BSP/UART/uart_irq.h" 

static int exp_reboot_callback(void) {
    NVIC_SystemReset();
    return 0; 
}

static int exp_shutdown_callback(void) {
    csp_log_error("Shutdown requested, but not implemented. System will continue running.");
    return 0;
}

static void exp_csp_debug_hook(csp_debug_level_t level, const char *format, va_list args) {
    char buf[256];
    int len = vsnprintf(buf, sizeof(buf), format, args);
    
    if (len > 0) {
        for (int i = 0; i < len; i++) {
            UART2_WriteByte((uint8_t)buf[i]);
        }
        UART2_WriteByte((uint8_t)'\r');
        UART2_WriteByte((uint8_t)'\n');
    }
}

void HardwareCSP_Init(void) {
    csp_sys_set_reboot(exp_reboot_callback);
    csp_sys_set_shutdown(exp_shutdown_callback);
    csp_debug_hook_set(exp_csp_debug_hook);
}