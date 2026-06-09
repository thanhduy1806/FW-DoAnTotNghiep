/*
 * do.c
 *
 *  Created on: Aug 3, 2025
 *      Author: Admin
 */

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "peripheral/pio/plib_pio.h"               // SYS function prototypes
#include "do.h"

#include "M2_BSP/BSP_LSM6DSOX/bsp_lsm6dsox.h"
#include "../../../../M2_BSP/BSP_Led/bsp_led.h"
#include "../../../../../config/default/interrupts.h"

#define GPIO_MAX_PORTS 5  // A to E

#define PIO_PTR(x) ((pio_registers_t *)(x))

static pio_registers_t * const gpio_ports[GPIO_MAX_PORTS + 1] = {
    NULL,
    PIO_PTR(PIO_PORT_A),
    PIO_PTR(PIO_PORT_B),
    PIO_PTR(PIO_PORT_C),
    PIO_PTR(PIO_PORT_D),
    PIO_PTR(PIO_PORT_E),
};

static uint32_t const gpio_clock_en[GPIO_MAX_PORTS + 1] =
{
    0,
    ID_PIOA,
    ID_PIOB,
    ID_PIOC,
    ID_PIOD,
    ID_PIOE
};

static void gpio_enable_clock(uint32_t ui32PortNum)
{
    if (ui32PortNum == 0 || ui32PortNum > GPIO_MAX_PORTS)
    {
        return;
    }

    uint32_t periph_id = gpio_clock_en[ui32PortNum];

    if (periph_id < 32U)
    {
        PMC_REGS->PMC_PCER0 = (1UL << periph_id);
    }
    else
    {
        PMC_REGS->PMC_PCER1 = (1UL << (periph_id - 32U));
    }
}

void do_set(do_t *me)
{
    if (!me) return;
   
    pio_registers_t *port = gpio_ports[me->port];   
    uint32_t mask = (1u << me->pin);
    
//    port->PIO_PER = mask;
//
//    port->PIO_OER = mask;

    port->PIO_SODR = mask;

    me->bStatus = true;
}

void do_reset(do_t *me)
{
    if (!me) return;

    pio_registers_t *port = gpio_ports[me->port];
    uint32_t mask = (1u << me->pin);
    
//    port->PIO_PER = mask;
//
//    port->PIO_OER = mask;

    port->PIO_CODR = mask;

    me->bStatus = false;
}

void do_toggle(do_t *me)
{
    if (!me) return;
   
    pio_registers_t *port = gpio_ports[me->port];
    uint32_t mask = (1u << me->pin);
   
//    port->PIO_PER = mask;
//
//    port->PIO_OER = mask;
    
    if(((port->PIO_ODSR) & mask) != 0u)
    {
        port->PIO_CODR = mask;
        me->bStatus = false;
    }else{
        port->PIO_SODR = mask;
        me->bStatus = true;
    }
}

void PIOC_Handler(void)
{
    uint32_t status = ((pio_registers_t*)PIO_PORT_C)->PIO_ISR;
    if (status & (1U << 21)) 
    {
        bsp_lsm6dsox_callback();
    }
}