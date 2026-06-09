#ifndef _BSP_SOL_DUAL_H_
#define _BSP_SOL_DUAL_H_

#include "os.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "M2_BSP/BSP_Board/board_v71_satellite.h"
#include "../../M3_Driver/components/dio/do.h"
#include "bsp_core.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct {
    do_t *in1;
    do_t *in2;
} sol_dual_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef enum {
    SOL_PIN_1_IN1 = 0,
    SOL_PIN_1_IN2,
    SOL_PIN_2_IN1,
    SOL_PIN_2_IN2,
    SOL_PIN_3_IN1,
    SOL_PIN_3_IN2,
    SOL_PIN_4_IN1,
    SOL_PIN_4_IN2,
} sol_pin_id_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Extern ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

extern sol_dual_t sol_dual[4];
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void bsp_sol_dual_forward(sol_dual_t *sol);
void bsp_sol_dual_reverse(sol_dual_t *sol);

#endif