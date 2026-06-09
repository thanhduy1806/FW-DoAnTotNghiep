#ifndef _BOARD_V71_ISS_H_
#define _BOARD_V71_ISS_H_


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "samv71q21b.h"

#define LASER_DAC_LATCH_PORT    3    //PC16
#define LASER_DAC_LATCH_PIN     16   //PC16 

#define LASER_DAC_CS_PORT    1    //PORTA
#define LASER_DAC_CS_PIN     31   //PA31   

#define LASER_SW_INT_CS_PORT    3   //PC07  
#define LASER_SW_INT_CS_PIN     7   //PC07  

#define LASER_SW_EXT_CS_PORT    1   //PA16  
#define LASER_SW_EXT_CS_PIN     16   //PA16  
#endif