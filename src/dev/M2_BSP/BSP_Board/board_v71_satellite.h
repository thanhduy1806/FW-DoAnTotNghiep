/* 
 * File:   board_v71_satellite.h
 * Author: HTSANG
 *
 * Created on February 27, 2026, 11:24 AM
 */

#ifndef BOARD_V71_SATELLITE_H
#define BOARD_V71_SATELLITE_H

#include "samv71q21b.h"


/* ===================== POWER CONTROL ===================== */

#define POWER_SOM_PORT        4   // PD5
#define POWER_SOM_PIN         5

#define POWER_PERI_PORT       5   // PE4
#define POWER_PERI_PIN        4

#define POWER_TEC_PORT        4   // PD4
#define POWER_TEC_PIN         4

#define POWER_HD4_PORT        4   // PD3
#define POWER_HD4_PIN         3

#define POWER_SLN_PORT        4   // PD2
#define POWER_SLN_PIN         2

#define POWER_LP_PORT         4   // PD6
#define POWER_LP_PIN          6

#define POWER_HEATER_PORT     5   // PE3
#define POWER_HEATER_PIN      3


/* ===================== LASER ===================== */

#define LASER_DAC_LATCH_PORT  3   // PC16
#define LASER_DAC_LATCH_PIN   16

#define LASER_DAC_CS_PORT     1   // PA31
#define LASER_DAC_CS_PIN      31

#define LASER_SW_INT_CS_PORT  3   // PC07
#define LASER_SW_INT_CS_PIN   7

#define LASER_SW_EXT_CS_PORT  1   // PA16
#define LASER_SW_EXT_CS_PIN   16

/* ===================== PHOTO ===================== */

#define PHOTO_SW_CS_PORT      4   // PD12
#define PHOTO_SW_CS_PIN       12

#define PHOTO_ADC_CS_PORT     4   // PD0
#define PHOTO_ADC_CS_PIN      0

#define PHOTO_ADC_CONV_PORT   1   // PA15
#define PHOTO_ADC_CONV_PIN    15

#define PHOTO_ADC_EOC_PORT    1   // PA18
#define PHOTO_ADC_EOC_PIN     18

/* ===================== SPI0 ===================== */

#define SPI0_SCK_PORT         4   // PD22
#define SPI0_SCK_PIN          22

#define SPI0_MOSI_PORT        4   // PD21
#define SPI0_MOSI_PIN         21

#define SPI0_MISO_PORT        4   // PD20
#define SPI0_MISO_PIN         20


/* ===================== STATUS ===================== */

#define STATUS_LED_PORT       2   // PB12
#define STATUS_LED_PIN        12


/* ===================== HEATER PWM ===================== */

#define HTR_PWM_1_PORT        1   // PA27
#define HTR_PWM_1_PIN         27

#define HTR_PWM_2_PORT        1   // PA00
#define HTR_PWM_2_PIN         0

#define HTR_PWM_3_PORT        1   // PA01
#define HTR_PWM_3_PIN         1

#define HTR_PWM_4_PORT        1   // PA26
#define HTR_PWM_4_PIN         26

#define HTR_PWM_5_PORT        3   // PC08
#define HTR_PWM_5_PIN         8

#define HTR_PWM_6_PORT        3   // PC23
#define HTR_PWM_6_PIN         23

#define HTR_PWM_7_PORT        3   // PC05
#define HTR_PWM_7_PIN         5

#define HTR_PWM_8_PORT        3   // PC06
#define HTR_PWM_8_PIN         6


/* ===================== TEC ===================== */

#define TEC_1_CS_PORT         3   // PC18
#define TEC_1_CS_PIN          18

#define TEC_1_SW_PORT         1   // PA30
#define TEC_1_SW_PIN          30


#define TEC_2_CS_PORT         3   // PC20
#define TEC_2_CS_PIN          20

#define TEC_2_SW_PORT         4   // PD13
#define TEC_2_SW_PIN          13


#define TEC_3_CS_PORT         3   // PC22
#define TEC_3_CS_PIN          22

#define TEC_3_SW_PORT         3   // PC01
#define TEC_3_SW_PIN          1


#define TEC_4_CS_PORT         4   // PD11
#define TEC_4_CS_PIN          11

#define TEC_4_SW_PORT         3   // PC02
#define TEC_4_SW_PIN          2


/* ===================== USART2 SPI ===================== */

#define USART2_SPI_SCK_PORT   4   // PD17
#define USART2_SPI_SCK_PIN    17

#define USART2_SPI_MOSI_PORT  4   // PD16
#define USART2_SPI_MOSI_PIN   16

#define USART2_SPI_MISO_PORT  4   // PD15
#define USART2_SPI_MISO_PIN   15


/* ===================== I2C ===================== */

#define I2C0_SDA_PORT         1   // PA03
#define I2C0_SDA_PIN          3

#define I2C0_SCL_PORT         1   // PA04
#define I2C0_SCL_PIN          4

#define I2C1_SDA_PORT         2   // PB04
#define I2C1_SDA_PIN          4

#define I2C1_SCL_PORT         2   // PB05
#define I2C1_SCL_PIN          5


/* ===================== SENSOR ===================== */

#define SENSOR5_EN_PORT       3   // PC11
#define SENSOR5_EN_PIN        11

#define FRAM_CS_PORT          4   // PD19
#define FRAM_CS_PIN           19

#define FRAM_CS_PORT          4   // PD19
#define FRAM_CS_PIN           19

#define TEC_3_SW_PORT         3  //PC01
#define TEC_3_SW_PIN          1   //PC01

#define TEC_4_CS_PORT         4  //PD11
#define TEC_4_CS_PIN          11   //PD11

#define TEC_4_SW_PORT         3  //PC02
#define TEC_4_SW_PIN          2   //PC02

/* ===================== NTC ===================== */

#define ADC_TEMP1_PORT        1  //PA19
#define ADC_TEMP1_PIN         19  //PA19

#define ADC_TEMP2_PORT        1  //PA20
#define ADC_TEMP2_PIN         20  //PA20

#define ADC_TEMP3_PORT        2  //PB0
#define ADC_TEMP3_PIN         0  //PB0

#define ADC_TEMP4_PORT        2  //PB1
#define ADC_TEMP4_PIN         1  //PB1

#define ADC_TEMP5_PORT        3  //PC15
#define ADC_TEMP5_PIN         15  //PC15

#define ADC_TEMP6_PORT        3  //PC13
#define ADC_TEMP6_PIN         13  //PC13

#define ADC_TEMP7_PORT        2  //PB2
#define ADC_TEMP7_PIN         2  //PB2

#define ADC_TEMP8_PORT        3  //PC29
#define ADC_TEMP8_PIN         29  //PC29

/* ===================== SOLENOID ===================== */

#define SOL1_IN1_PORT        1  //PA02
#define SOL1_IN1_PIN         2  //PA02

#define SOL1_IN2_PORT        3  //PC19
#define SOL1_IN2_PIN         19 //PC19

#define SOL2_IN1_PORT        4  //PD18
#define SOL2_IN1_PIN         18 //PD18

#define SOL2_IN2_PORT        5  //PE05
#define SOL2_IN2_PIN         5  //PE05

#define SOL3_IN1_PORT        1  //PA23
#define SOL3_IN1_PIN         23 //PA23

#define SOL3_IN2_PORT        1  //PA22
#define SOL3_IN2_PIN         22 //PA22

#define SOL4_IN1_PORT        1  //PA25
#define SOL4_IN1_PIN         25 //PA25

#define SOL4_IN2_PORT        1  //PA24
#define SOL4_IN2_PIN         24 //PA24

/* ===================== PUMP ===================== */
#define PUMP_EN_PORT        3  //PC17
#define PUMP_EN_PIN         17 //PC17

/* ============= SW SPI FOR PSRAM ================= */
#define SW_PSRAM_SEL_PORT    4
#define SW_PSRAM_SEL_PIN     14

/* ===================== SPI0 ===================== */
#define PSRAM_CS_PORT         3   // PC28
#define PSRAM_CS_PIN          28

#define SPI1_SCK_PORT         3   // PC24
#define SPI1_SCK_PIN          24

#define SPI1_MOSI_PORT        3   // PC27
#define SPI1_MOSI_PIN         27

#define SPI1_MISO_PORT        3   // PC26
#define SPI1_MISO_PIN         26

/* ===================== I2C FLOW_SENSOR ===================== */
#define FLOW_1_EN_PORT        3   // PC9
#define FLOW_1_EN_PIN         9

#define FLOW_2_EN_PORT        3   // PC10
#define FLOW_2_EN_PIN         10

#define FLOW_3_EN_PORT        1   // PA28
#define FLOW_3_EN_PIN         28

#define FLOW_4_EN_PORT        1   // PA29
#define FLOW_4_EN_PIN         29

/* ===================== LSM6DSOX ===================== */
#define LSM6_INT1_PORT        3   // PC21
#define LSM6_INT1_PIN         21
/* ==================== UART0 ===================== */
#define UART0_RX_PORT         1   // PA9
#define UART0_RX_PIN          9

#define UART0_TX_PORT         1   // PA10
#define UART0_TX_PIN          10

/* ==================== WATCHDOG ===================== */
#define WDT_DONE_PORT      5   //  PE1
#define WDT_DONE_PIN       1

#define WDT_WAKE_PORT      5   //  PE2
#define WDT_WAKE_PIN       2

/* =====================GPIO MCU-PMU==================*/
#define MCU_MPU_GPIO_A_PORT     4
#define MCU_MPU_GPIO_A_PIN      9

#define MCU_MPU_GPIO_B_PORT     4
#define MCU_MPU_GPIO_B_PIN      10

#endif /* BOARD_V71_SATELLITE_H */
