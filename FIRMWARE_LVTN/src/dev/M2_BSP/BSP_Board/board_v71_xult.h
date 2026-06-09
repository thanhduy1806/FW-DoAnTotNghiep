#ifndef _BOARD_V71_XULT_H_
#define _BOARD_V71_XULT_H_

#define LASER_DAC_LATCH_PORT   3    //PC16
#define LASER_DAC_LATCH_PIN    16   //PC16 

#define LASER_DAC_CS_PORT    1    //PA31
#define LASER_DAC_CS_PIN     31   //PA31   

#define LASER_SW_INT_CS_PORT   3   //PC07  
#define LASER_SW_INT_CS_PIN    7   //PC07  

#define LASER_SW_EXT_CS_PORT   1   //PA16  
#define LASER_SW_EXT_CS_PIN    16  //PA16  

#define PHOTO_ADC_CONV_PORT    1   //PA15 
#define PHOTO_ADC_CONV_PIN     15  //PA15

#define PHOTO_ADC_EOC_PORT     1   //PA18 
#define PHOTO_ADC_EOC_PIN      18  //PA18

#define PHOTO_ADC_CS_PORT      4   //PD0 
#define PHOTO_ADC_CS_PIN       0   //PD0

#define PHOTO_SPI0_CS_PORT     4   //PD12 
#define PHOTO_SPI0_CS_PIN      12  //PD12

#define STATUS_LED_PORT         1    //PA23
#define STATUS_LED_PIN          23   //PA23

#define POWER_SOM_PORT          4
#define POWER_SOM_PIN           5

#define POWER_PERI_PORT          5
#define POWER_PERI_PIN           4

#define POWER_TEC_PORT          4
#define POWER_TEC_PIN           4

#define POWER_HD4_PORT          4
#define POWER_HD4_PIN           3

#define POWER_SLN_PORT          4
#define POWER_SLN_PIN           2

#define POWER_LP_PORT          4
#define POWER_LP_PIN           6

#define POWER_HEATER_PORT       5
#define POWER_HEATER_PIN        3

#define I2C0_SDA_PORT           1    //PA03
#define I2C0_SDA_PIN            3    //PA03

#define I2C0_SCL_PORT           1    //PA04   
#define I2C0_SCL_PIN            4    //PA04

#define I2C1_SDA_PORT           2     //PB04
#define I2C1_SDA_PIN            4     //PB04

#define I2C1_SCL_PORT           2     //PB05
#define I2C1_SCL_PIN            5     //PB05

#define BMP390_ONBOARD_ADDRESS          0x76
#define BMP390_CONNECTOR_ADDRESS        0x77

#define SENSOR5_EN_PORT         3   //PC11
#define SENSOR5_EN_PIN          11  //PC11


#endif /* #ifndef _BOARD_V71_XULT_H_ */