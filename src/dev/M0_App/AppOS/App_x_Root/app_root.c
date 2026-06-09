/*
 * app_root.c
 *
 *  Created on: Feb 21, 2025
 *      Author: CAO HIEU
 */

#include "os.h"
#include "stdio.h"

#include "app_root.h"
#include "M0_App/AppOS/App_4_Alive/app_alive.h"
#include "M0_App/AppOS/App_3_CLI/app_cli.h"
#include "M0_App/AppOS/App_4_Logging/app_logging.h"
#include "M0_App/AppOS/App_3_Network/app_network.h"
#include "M0_App/AppOS/App_3_USB/app_usb.h"
#include "M0_App/AppOS/App_3_CSP/app_csp.h"
#include "M0_App/AppOS/App_3_XTP/app_xtp.h"
#include "M0_App/AppOS/App_4_CTRL/app_ctrl.h"
#include "M0_App/AppOS/App_4_NTC/app_ntc.h"
#include "M0_App/AppOS/App_4_Init/app_init.h"
#include "M0_App/AppOS/App_6_Temperature_Control/app_temperature.h"

#include "M1_SysApp/xlog/xlog.h"
#include "M1_SysApp/dmesg/dmesg.h"
#include "M0_App/AppOS/App_Experiment/app_experiment.h"

#define ALIVETASK_TIMEOUT_MS    4000
#define CLITASK_TIMEOUT_MS      100
#define LOGTASK_TIMEOUT_MS      500
#define USBTASK_TIMEOUT_MS      1000
#define NETWORKTASK_TIMEOUT_MS  1000
#define CSPTASK_TIMEOUT_MS      1000
#define CTRLTASK_TIMEOUT_MS     2000
#define NTCTASK_TIMEOUT_MS      1000
#define TEMPTASK_TIMEOUT_MS     1000
#define EXPTASK_TIMEOUT_MS      1000
#define PSRAMTASK_TIMEOUT_MS    1000
#define APP_XTP_TIMEOUT_MS      1000

/**
 * @brief  Initialize and create all RTOS tasks and objects
 * | Task Name    | Priority | WDT Timeout (ms) | Description / Function                |
 * |--------------|:--------:|:----------------:|---------------------------------------|
 * | Root Init    |    1     | N/A              | System init (AppRoot_GrowUp)          |
 * | Monitor      |    1     | N/A              | Task watchdog & system health         |
 * | CLI          |    3     | 100              | Command Line Interface                |
 * | USB_Task     |    3     | 1000             | USB Device / Host stack               |
 * | Network_Task |    3     | 1000             | TCP/IP & network services             |
 * | CSP_Task     |    3     | 1000             | CSP protocol over CAN                 |
 * | Ctrl_Task    |    3     | 2000             | Control logic / param table           |
 * | XTP_Task     |    3     | 1000             | XTP communication stack               |
 * | LED_Alive    |    4     | 4000             | Heartbeat / status LED                |
 * | Log_Task     |    4     | 500              | Logging / flash write                 |
 * | NTC_Task     |    4     | 4000             | Temperature sensing                   |
 * | EXP_Task     |    4     | 1000             | Experiment logic                      |
 * | PSRAM_Task   |    ?     | 1000             | PSRAM handling (check priority)       |
 * | IDLE         |    0     | N/A              | Idle task (RTOS internal)             |
 */

static void AliveTimeoutCallback(void) {
}

static void CLITimeoutCallback(void) {
}

void AppRoot_GrowUp(void) {
    xlog_Init();
    Dmesg_Init();
    osThreadNew_WithMonitor(App_AliveTask, NULL, &alive_attr, ALIVETASK_TIMEOUT_MS, AliveTimeoutCallback);
    osThreadNew_WithMonitor(App_CLITask, NULL, &cli_attr, CLITASK_TIMEOUT_MS, CLITimeoutCallback);
    osThreadNew_WithMonitor(App_LogTask, NULL, &log_attr, LOGTASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_USBTask, NULL, &usb_attr, USBTASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_NetworkTask, NULL, &network_attr, NETWORKTASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_CSPTask, NULL, &csp_attr, CSPTASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_CtrlTask, NULL, &ctrl_attr, CTRLTASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_NTCTask, NULL, &ntc_attr, ALIVETASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_TemperatureTask, NULL, &temp_attr, TEMPTASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_EXPTask, NULL, &exp_attr, EXPTASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(APP_PSRAMTask, NULL, &psram_attr, PSRAMTASK_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_XTPTask, NULL, &xtp_attr, APP_XTP_TIMEOUT_MS, NULL);
    osThreadNew_WithMonitor(App_InitTask, NULL, &init_attr, ALIVETASK_TIMEOUT_MS, NULL);
}

void App_Start(void) {
    osRegisterRootInit(AppRoot_GrowUp);
    osKernelStart();
}