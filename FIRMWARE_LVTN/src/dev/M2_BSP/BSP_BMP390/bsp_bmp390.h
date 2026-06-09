#ifndef BSP_BSP_BMP390_H_
#define BSP_BSP_BMP390_H_

#include "define.h"
#include "bmp390.h"
#include "i2c_io.h"

extern bmp390_dev_t bmp390_int;
extern bmp390_dev_t bmp390_ext;

uint32_t bsp_bmp390_init(bmp390_dev_t* dev);
uint32_t bsp_bmp390_read(bmp390_dev_t* dev, bmp390_data_t* p_data);

void bsp_bmp390_ena_switch(void);
void bsp_bmp390_dis_switch(void);
bool bsp_bmp390_status_switch(void);

#endif