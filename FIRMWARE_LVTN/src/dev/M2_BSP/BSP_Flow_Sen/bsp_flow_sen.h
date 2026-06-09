/* 
 * File:   bsp_flow_sen.h
 * Author: HTSANG
 *
 * Created on April 7, 2026, 2:20 PM
 */

#ifndef BSP_FLOW_SEN_H
#define	BSP_FLOW_SEN_H

#include "M3_Driver/devices/slf3s_flow/slf3s_flow.h"

typedef struct flow_sensor_data {
    slf3s_readings_t slf3s_dat;
    bool read_oke;
} flow_sensor_data_t;

extern flow_sensor_data_t s_flow_sen_data[4];

int bsp_flow_sen_init (void);

int bsp_flow_sen_read_all (void);

#endif	/* BSP_FLOW_SEN_H */

