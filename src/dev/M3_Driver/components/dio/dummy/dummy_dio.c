#include "do.h"

__attribute__(( weak )) void do_set(do_t *me) 
{
  // Dummy implementation: Set the status to true
  me->bStatus = true;
  return;
}
__attribute__(( weak )) void do_reset(do_t *me) 
{
  me->bStatus = false;
   return;
}
__attribute__(( weak )) void do_toggle(do_t *me) 
{
  me->bStatus = !me->bStatus;
   return;
}