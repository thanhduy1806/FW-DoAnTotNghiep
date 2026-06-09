/*
 * os.c
 *
 *  Created on: Feb 21, 2025
 *      Author: CAO HIEU
 */

#include "os.h"
#include "stdio.h"
#include "board.h"
/*-----------------Pre-Define----------------*/
#define ROOT_STACK_SIZE 512
static StackType_t root_stack[ROOT_STACK_SIZE];
static StaticTask_t root_tcb;


static osMonitorEntry_t monitor_list[MAX_MONITOR_TASKS];
static uint8_t monitor_count = 0;

#define MONITOR_STACK_SIZE 256
static StackType_t monitor_stack[MONITOR_STACK_SIZE];
static StaticTask_t monitor_tcb;

/*--------------------Root Task--------------*/
static osRootInitCallback_t g_root_init_callback = NULL;

void osRegisterRootInit(osRootInitCallback_t callback)
{
    g_root_init_callback = callback;
}

static void osRootTask(void *param)
{
    if (g_root_init_callback != NULL) {
        g_root_init_callback();
    }
    
    vTaskDelete(NULL);
}

/*--------------------RTOS Task List--------------*/

/* Hook prototypes */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Stack Overflow -> Task %s", pcTaskName);
    DEBUG_SendString(buffer);
}

void vApplicationMallocFailedHook(void)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Malloc Hook Overflow");
	DEBUG_SendString(buffer);
}

/*--------------------Monitor--------------*/
static void osMonitorTask(void *param) {
    (void)param; // Unused
    const TickType_t period = pdMS_TO_TICKS(100); 

    while (1) {
        TickType_t current_tick = xTaskGetTickCount();
        bool system_ok = true;

        for (int i = 0; i < monitor_count; i++) {
            TickType_t elapsed = current_tick - monitor_list[i].last_checkin_tick;
            
            if (elapsed > monitor_list[i].timeout_ticks) {
                
                // (Log/Signal)
                if (monitor_list[i].callback) {
                    monitor_list[i].callback();
                }
                
                system_ok = false; 
                // printf("Task ID %p Timeout! Elapsed: %u\r\n", monitor_list[i].thread_id, elapsed);
            }
        }

        if (system_ok) {
            // HAL_IWDG_Refresh(&hiwdg);
        }
        
        vTaskDelay(period);
    }
}

osThreadId_t osThreadNew_WithMonitor(void (*func)(void *), void *argument, const osThreadAttr_t *attr, uint32_t timeout_ms, void (*watch_cb)(void)) {
    osThreadId_t tid = osThreadNew(func, argument, attr);
    
    if (tid != NULL && monitor_count < MAX_MONITOR_TASKS) {
        taskENTER_CRITICAL();
        monitor_list[monitor_count].thread_id = tid;
        monitor_list[monitor_count].timeout_ticks = pdMS_TO_TICKS(timeout_ms);
        monitor_list[monitor_count].last_checkin_tick = xTaskGetTickCount();
        monitor_list[monitor_count].callback = watch_cb;
        monitor_count++;
        taskEXIT_CRITICAL();
    }
    return tid;
}

void osThreadFeed(void) {
    osThreadId_t current_tid = xTaskGetCurrentTaskHandle();
    for (int i = 0; i < monitor_count; i++) {
        if (monitor_list[i].thread_id == current_tid) {
            monitor_list[i].last_checkin_tick = xTaskGetTickCount();
            return;
        }
    }
}
/*--------------------OS Wrapper--------------*/
osThreadId_t osThreadNew(void (*func)(void *), void *argument, const osThreadAttr_t *attr) {
    if (func == NULL || attr == NULL) return NULL;

    osThreadId_t handle = NULL;

    if (attr->stack_mem != NULL && attr->cb_mem != NULL) {
        handle = xTaskCreateStatic(
            func,
            attr->name,
            attr->stack_size,   // Word
            argument,
            attr->priority,
            (StackType_t *)attr->stack_mem,
            attr->cb_mem
        );
    } 
    else {
        if (xTaskCreate(func, attr->name, (uint16_t)attr->stack_size, argument, attr->priority, &handle) != pdPASS) {
            return NULL;
        }
    }
    return handle;
}

void osKernelStart(void) {
    osThreadAttr_t root_attr = {
        .name = "Root",
        .stack_size = ROOT_STACK_SIZE,
        .priority = configMAX_PRIORITIES - 1,  // Highest priority
        .stack_mem = root_stack,    
        .cb_mem = &root_tcb  
    };
    osThreadNew(osRootTask, NULL, &root_attr);
    
    osThreadAttr_t monitor_attr = {
            .name = "Monitor", 
            .stack_size = MONITOR_STACK_SIZE, 
            .priority = configMAX_PRIORITIES - 1,
            .stack_mem = monitor_stack,    
            .cb_mem = &monitor_tcb             
    };
    osThreadNew(osMonitorTask, NULL, &monitor_attr);
     
    vTaskStartScheduler();
}

void osDelay(uint32_t ticks) {
    vTaskDelay(ticks);
}

void osDelayUntil(TickType_t *previous_wake_time, uint32_t ticks) {
    vTaskDelayUntil(previous_wake_time, ticks);
}

void osThreadExit(void) {
    vTaskDelete(NULL);
}

uint32_t osKernelGetTickCount(void) {
    return xTaskGetTickCount();
}   