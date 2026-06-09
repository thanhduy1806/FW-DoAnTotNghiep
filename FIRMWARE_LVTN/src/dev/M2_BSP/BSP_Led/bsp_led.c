#include "bsp_led.h"
#include "bsp_core.h"


void bsp_led_set(void)
{
    do_set(&status_led);
}

void bsp_led_reset(void)
{
    do_reset(&status_led);
}

void bsp_led_toggle(void)
{
    do_toggle(&status_led);
}
