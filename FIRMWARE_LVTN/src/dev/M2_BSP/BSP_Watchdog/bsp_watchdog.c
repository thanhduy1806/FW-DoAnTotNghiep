#include "bsp_watchdog.h"
#include "M5_Utils/Tick/tick.h"

void bsp_watchdog_init(void)
{
    tpl5010_init(&wdt_done);
}

void bsp_watchdog_update(uint32_t now)
{
    /* --- Watchdog: HIGH / LOW --- */
    static uint32_t wd_last = 0U;

    uint32_t wd_period = (tpl5010_get_state() == TPL5010_STATE_HIGH)
                         ? WD_HIGH_PERIOD_MS     /* 200ms */
                         : WD_LOW_PERIOD_MS;     /* 600ms */

    if ((now - wd_last) >= wd_period)
    {
        wd_last = now;
        tpl5010_feed(&wdt_done);
    }
}