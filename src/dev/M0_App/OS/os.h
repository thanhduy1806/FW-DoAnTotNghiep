/*
 * rtos.h
 *
 *  Created on: Feb 21, 2025
 *      Author: CAO HIEU
 */

#ifndef APP_RTOS_RTOS_H_
#define APP_RTOS_RTOS_H_
#include "stdint.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAX_MONITOR_TASKS   16

typedef TaskHandle_t osThreadId_t;

typedef struct {
    const char *name;          
    uint32_t stack_size;        // Stack Size (Word)
    uint32_t priority;          
    void *stack_mem;            // Pointer Stack (NULL if Dynamic)
    StaticTask_t *cb_mem;       // Pointer TCB (NULL if Dynamic)
} osThreadAttr_t;

typedef struct {
    osThreadId_t thread_id;
    uint32_t timeout_ticks;
    uint32_t last_checkin_tick;
    void (*callback)(void); // Monitor Notify Function
} osMonitorEntry_t;

/* Core OS wrapper functions */
osThreadId_t osThreadNew(void (*func)(void *), void *argument, const osThreadAttr_t *attr);
void         osKernelStart(void);
void         osDelay(uint32_t ms);
void         osDelayUntil(TickType_t *previous_wake_time, uint32_t ticks);
void         osThreadExit(void);

/* Monitor functions */
osThreadId_t osThreadNew_WithMonitor(void (*func)(void *), void *argument, 
                                     const osThreadAttr_t *attr, 
                                     uint32_t timeout_ms, 
                                     void (*watch_cb)(void));
void         osThreadFeed(void);

/* Root task registration */
typedef void (*osRootInitCallback_t)(void);

void         osRegisterRootInit(osRootInitCallback_t callback);
uint32_t osKernelGetTickCount(void);

#endif /* APP_RTOS_RTOS_H_ */
