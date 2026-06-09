/* 
 * File:   bsp_tec.h
 * Author: HTSANG
 *
 * Created on March 10, 2026, 10:10 AM
 */

#ifndef BSP_TEC_H
#define	BSP_TEC_H

#include "../../M3_Driver/devices/LT8722/lt8722.h"
#include "../BSP_Board/bsp_core.h"

#define tec_dev_t     lt8722_dev_t
#define TEC_MAX_NUM   4

extern tec_dev_t* p_tec[4];

uint32_t bsp_tec_init(tec_dev_t* dev);
uint32_t bsp_tec_clear_status(tec_dev_t* dev);
uint32_t bsp_tec_get_status(tec_dev_t* dev, uint16_t *status);
uint32_t bsp_tec_set_output_voltage_channel(tec_dev_t* dev, int64_t nVol);
uint32_t bsp_tec_set_enable_req(tec_dev_t* dev);
uint32_t bsp_tec_set_swen_req(tec_dev_t* dev);
uint32_t bsp_tec_clear_swen_req(tec_dev_t* dev);


#endif	/* BSP_TEC_H */

