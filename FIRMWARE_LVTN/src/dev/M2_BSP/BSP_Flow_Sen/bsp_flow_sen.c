#include "bsp_flow_sen.h"
#include "stdbool.h"
#include "do.h"
#include "bsp_core.h"


flow_sensor_data_t s_flow_sen_data[4];

int bsp_flow_sen_init (void)
{
    do_set(&flow1_en);
    slf3s_init(&i2c0, true);
    do_reset(&flow1_en);
    
    do_set(&flow2_en);
    slf3s_init(&i2c0, true);
    do_reset(&flow2_en);
    
    do_set(&flow3_en);
    slf3s_init(&i2c0, true);
    do_reset(&flow3_en);
    
    do_set(&flow4_en);
    slf3s_init(&i2c0, true);
    do_reset(&flow4_en);
    
    return 1;
}

int bsp_flow_sen_read_all (void)
{
    do_set(&flow1_en);
    int32_t ret = slf3s_read_all(&i2c0, &s_flow_sen_data[0].slf3s_dat);
    do_reset(&flow1_en);
    if (ret == SLF3S_OK)
        s_flow_sen_data[0].read_oke = true;
    else s_flow_sen_data[0].read_oke = false;
    
    do_set(&flow2_en);
    ret = slf3s_read_all(&i2c0, &s_flow_sen_data[1].slf3s_dat);
    do_reset(&flow2_en);
    if (ret == SLF3S_OK)
        s_flow_sen_data[1].read_oke = true;
    else s_flow_sen_data[1].read_oke = false;
        
    do_set(&flow3_en);
    ret = slf3s_read_all(&i2c0, &s_flow_sen_data[2].slf3s_dat);
    do_reset(&flow3_en);
    if (ret == SLF3S_OK)
        s_flow_sen_data[2].read_oke = true;
    else s_flow_sen_data[2].read_oke = false;
        
    do_set(&flow4_en);
    ret = slf3s_read_all(&i2c0, &s_flow_sen_data[3].slf3s_dat);
    do_reset(&flow4_en);
    if (ret == SLF3S_OK)
        s_flow_sen_data[3].read_oke = true;
    else s_flow_sen_data[3].read_oke = false;
    
    return (  s_flow_sen_data[0].read_oke || s_flow_sen_data[1].read_oke
           || s_flow_sen_data[2].read_oke || s_flow_sen_data[3].read_oke);
}
