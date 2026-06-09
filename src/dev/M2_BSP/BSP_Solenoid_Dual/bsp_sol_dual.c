#include "M2_BSP/BSP_Solenoid_Dual/bsp_sol_dual.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Array ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
sol_dual_t sol_dual[4] = {
    { .in1 = &sol1_in1, .in2 = &sol1_in2 },
    { .in1 = &sol2_in1, .in2 = &sol2_in2 },
    { .in1 = &sol3_in1, .in2 = &sol3_in2 },
    { .in1 = &sol4_in1,  .in2 = &sol4_in2 },
};
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void bsp_sol_dual_forward(sol_dual_t *sol)
{
    do_set(sol->in1);
    do_reset(sol->in2);
    
    osDelay(pdMS_TO_TICKS(30));
    
    do_reset(sol->in1);
    do_reset(sol->in2);
}

void bsp_sol_dual_reverse(sol_dual_t *sol)
{
    do_set(sol->in2);
    do_reset(sol->in1);
    
    osDelay(pdMS_TO_TICKS(30));
    
    do_reset(sol->in1);
    do_reset(sol->in2);
}