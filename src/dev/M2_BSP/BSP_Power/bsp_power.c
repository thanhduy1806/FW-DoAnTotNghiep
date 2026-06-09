/*
 * bsp_power.c
 *
 *  Created on: Feb 27, 2026
 *      Author: Admin
 */

#include "bsp_power.h"
#include "do.h"
#include "bsp_core.h"

//bsp_power_som
void bsp_power_som_on(void)
{
    do_set(&power_som);
}

void bsp_power_som_off(void)
{
    do_reset(&power_som);
}

bool bsp_power_som_status(void)
{
    return power_som.bStatus;
}

void bsp_power_buck_peri_on(void)
{
    do_set(&power_buck_peri);
}

void bsp_power_buck_peri_off(void)
{
    do_reset(&power_buck_peri);
}

bool bsp_power_buck_peri_status(void)
{
    return power_buck_peri.bStatus;
}

//bsp_power_tec
void bsp_power_tec_on(void)
{
    do_set(&power_buck_peri);
    do_set(&power_tec);
}

void bsp_power_tec_off(void)
{
    do_reset(&power_tec);
}

bool bsp_power_tec_status(void)
{
    return power_tec.bStatus;
}

//bsp_power_hd4
void bsp_power_hd4_on(void)
{
    do_set(&power_hd4);
}

void bsp_power_hd4_off(void)
{
    do_reset(&power_hd4);
}

bool bsp_power_hd4_status(void)
{
    return power_hd4.bStatus;
}

//bsp_power_solenoid
void bsp_power_solenoid_on(void)
{
    do_set(&power_solenoid);
}

void bsp_power_solenoid_off(void)
{
    do_reset(&power_solenoid);
}

bool bsp_power_solenoid_status(void)
{
    return power_solenoid.bStatus;
}

//bsp_power_lp
void bsp_power_lp_on(void)
{
    do_set(&power_lp);
}

void bsp_power_lp_off(void)
{
    do_reset(&power_lp);
}

bool bsp_power_lp_status(void)
{
    return power_lp.bStatus;
}

//bsp_power_heater
void bsp_power_heater_on(void)
{
    do_set(&power_heater);
}

void bsp_power_heater_off(void)
{
    do_reset(&power_heater);
}

bool bsp_power_heater_status(void)
{
    return power_heater.bStatus;
}


void power_on_all(void)
{
    bsp_power_som_on();
    bsp_power_buck_peri_on();
    bsp_power_tec_on();
    bsp_power_hd4_on();
    bsp_power_solenoid_on();
    bsp_power_lp_on();
    bsp_power_heater_on();
}

void power_off_all(void)
{
    bsp_power_som_off();
    bsp_power_buck_peri_off();
    bsp_power_tec_off();
    bsp_power_hd4_off();
    bsp_power_solenoid_off();
    bsp_power_lp_off();
    bsp_power_heater_off();
}

void power_status_all(void)
{
    bsp_power_som_status();
    bsp_power_buck_peri_status();
    bsp_power_tec_status();
    bsp_power_hd4_status();
    bsp_power_solenoid_status();
    bsp_power_lp_status();
    bsp_power_heater_status();
}
