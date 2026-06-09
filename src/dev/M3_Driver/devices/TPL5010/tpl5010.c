/*
 * wd_tpl5010.c
 */

#include "tpl5010.h"
#include "definitions.h"     

static tpl5010_state_t currentState = TPL5010_STATE_LOW;

bool tpl5010_init(do_t *wd_do)
{
    currentState = TPL5010_STATE_LOW;
    do_reset(wd_do);
    return true;
}

void tpl5010_feed(do_t *wd_do)
{
    if (currentState == TPL5010_STATE_LOW)
    {
        currentState = TPL5010_STATE_HIGH;
        do_set(wd_do); 
    }
    else
    {
        currentState = TPL5010_STATE_LOW;
        do_reset(wd_do);    
    }
}

tpl5010_state_t tpl5010_get_state(void)
{
    return currentState;
}