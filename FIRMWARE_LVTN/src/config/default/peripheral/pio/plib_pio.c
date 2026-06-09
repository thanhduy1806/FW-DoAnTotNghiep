/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pio.c

  Summary:
    PIO function implementations for the PIO PLIB.

  Description:
    The PIO PLIB provides a simple interface to manage peripheral
    input-output controller.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

#include "plib_pio.h"
#include "interrupts.h"
#include "../../M2_BSP/BSP_Board/board.h"



/******************************************************************************
  Function:
    PIO_Initialize ( void )

  Summary:
    Initialize the PIO library.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_Initialize ( void )
{
    MATRIX_REGS->CCFG_SYSIO = 0x0;
    /* Selected System IO pins are configured as GPIO */
    MATRIX_REGS->CCFG_SYSIO |= ((1U << I2C1_SDA_PIN) | (1U << I2C1_SCL_PIN) | (1U << STATUS_LED_PIN));

    /************************ PIO A Initialization ************************/
    //------begin-------
    /* PORTA PIO: Default*/
    ((pio_registers_t*)PIO_PORT_A)->PIO_ABCDSR[0] = 0x0U;
    ((pio_registers_t*)PIO_PORT_A)->PIO_ABCDSR[1] = 0x0U;
    uint32_t PIOA_Output_mask = ( (1U << TEC_1_SW_PIN) | (1U << LASER_DAC_CS_PIN) | (1U << LASER_SW_EXT_CS_PIN)
                                | (1U << SOL1_IN1_PIN) | (1U << SOL3_IN1_PIN) | (1U << SOL3_IN2_PIN)
                                | (1U << SOL4_IN1_PIN) | (1U << SOL4_IN2_PIN) | (1U << FLOW_3_EN_PIN) | (1U << FLOW_4_EN_PIN) );
    /* Select Peripheral A for PA */
    uint32_t PIOA_periph_A_mask = ((1U << I2C0_SDA_PIN) | (1U << I2C0_SCL_PIN) | (1U << UART0_RX_PIN) | (1U << UART0_TX_PIN));
    /* Select Peripheral B for PA00, PA01, PA26, PA27 */
    uint32_t PIOA_periph_B_mask = (1U << HTR_PWM_2_PIN) | (1U << HTR_PWM_3_PIN) | (1U << HTR_PWM_4_PIN) | (1U << HTR_PWM_1_PIN) | (1U << PHOTO_ADC_CONV_PIN);
    /* Select ADC for PA.. */
    uint32_t PIOA_ADC_mask = (1U << ADC_TEMP1_PIN) | (1U << ADC_TEMP2_PIN);
    ((pio_registers_t*)PIO_PORT_A)->PIO_ABCDSR[0] |=  PIOA_periph_B_mask;
    ((pio_registers_t*)PIO_PORT_A)->PIO_ABCDSR[1] &= ~PIOA_periph_B_mask;
    //------stop--------
    ((pio_registers_t*)PIO_PORT_A)->PIO_PER = 0xFFFFFFFFU;
    //------begin-------
    /* PORTA PIO Enable and Peripheral Disable*/
    ((pio_registers_t*)PIO_PORT_A)->PIO_PER &= ~(PIOA_periph_A_mask | PIOA_periph_B_mask | PIOA_ADC_mask);
    /* PORTA PIO Disable and Peripheral Enable*/
    ((pio_registers_t*)PIO_PORT_A)->PIO_PDR |=  (PIOA_periph_A_mask | PIOA_periph_B_mask | PIOA_ADC_mask);
    //------stop--------
    /* PORTA PIO Disable Interrupt */
    ((pio_registers_t*)PIO_PORT_A)->PIO_IDR |= PIOA_ADC_mask;
    ((pio_registers_t*)PIO_PORT_A)->PIO_MDDR = 0xFFFFFFFFU;
    /* PORTA Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_A)->PIO_PUDR = 0xFFFFFFFFU;
    ((pio_registers_t*)PIO_PORT_A)->PIO_PUDR |= PIOA_ADC_mask;
    /* PORTA Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_A)->PIO_PPDDR = 0xFFFFFFFFU;
    /* PORTA Output Write Enable */
    ((pio_registers_t*)PIO_PORT_A)->PIO_OWER = PIO_OWER_Msk;
    /* PORTA Output Direction Enable */
    ((pio_registers_t*)PIO_PORT_A)->PIO_OER = 0x0U;
    ((pio_registers_t*)PIO_PORT_A)->PIO_ODR = 0xFFFFFFFFU;
    //------begin-------
    ((pio_registers_t*)PIO_PORT_A)->PIO_OER |=  PIOA_Output_mask;
    ((pio_registers_t*)PIO_PORT_A)->PIO_ODR &= ~PIOA_Output_mask;
    //------stop--------
    /* Initialize PORTA pin state */
    ((pio_registers_t*)PIO_PORT_A)->PIO_ODSR = 0x0U;
    //------begin-------
//    ((pio_registers_t*)PIO_PORT_A)->PIO_ODSR |= ((1U << LASER_DAC_CS_PIN) | (1U << LASER_SW_EXT_CS_PIN));
    ((pio_registers_t*)PIO_PORT_A)->PIO_ODSR |= ((1U << LASER_DAC_CS_PIN) | (1U << LASER_SW_EXT_CS_PIN) | (1U << PHOTO_ADC_CONV_PIN));
    //------stop--------
    /* PORTA drive control */
    ((pio_registers_t*)PIO_PORT_A)->PIO_DRIVER = 0x0U;

    /************************ PIO B Initialization ************************/
    //------begin-------
    /* PORTB PIO: Default*/
    ((pio_registers_t*)PIO_PORT_B)->PIO_ABCDSR[0] = 0x0U;
    ((pio_registers_t*)PIO_PORT_B)->PIO_ABCDSR[1] = 0x0U;
    /* Select PIO_Output for PB */
    uint32_t PIOB_Output_mask = (1U << STATUS_LED_PIN);
    /* Select Peripheral A for PB */
    uint32_t PIOB_periph_A_mask = (1U << I2C1_SDA_PIN) | (1U << I2C1_SCL_PIN);
    uint32_t PIOB_ADC_mask = (1U << ADC_TEMP3_PIN) | (1U << ADC_TEMP4_PIN) | (1U << ADC_TEMP7_PIN);
    //------stop--------
    ((pio_registers_t*)PIO_PORT_B)->PIO_PER = 0xFFFFFFFFU;
    //------begin-------
    /* PORTB PIO Disable and Peripheral Enable*/
    ((pio_registers_t*)PIO_PORT_B)->PIO_PDR |=  (PIOB_periph_A_mask);
    /* PORTB PIO Enable and Peripheral Disable*/
    ((pio_registers_t*)PIO_PORT_B)->PIO_PER &= ~(PIOB_periph_A_mask);
    //------stop--------
    ((pio_registers_t*)PIO_PORT_B)->PIO_PDR |=  PIOB_ADC_mask;
    
    /* PORTA PIO Disable Interrupt */
    ((pio_registers_t*)PIO_PORT_B)->PIO_IDR |= PIOB_ADC_mask;
    
    ((pio_registers_t*)PIO_PORT_B)->PIO_MDDR = 0xFFFFFFFFU;
    /* PORTB Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_B)->PIO_PUDR = 0xFFFFFFFFU;
    ((pio_registers_t*)PIO_PORT_B)->PIO_PUDR |=  PIOB_ADC_mask;
    /* PORTB Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_B)->PIO_PPDDR = 0xFFFFFFFFU;
    /* PORTB Output Write Enable */
    ((pio_registers_t*)PIO_PORT_B)->PIO_OWER = PIO_OWER_Msk;
    /* PORTB Output Direction Enable */
    ((pio_registers_t*)PIO_PORT_B)->PIO_OER = 0x0U;
    ((pio_registers_t*)PIO_PORT_B)->PIO_ODR = 0xFFFFFFFFU;
    //------begin-------
    ((pio_registers_t*)PIO_PORT_B)->PIO_OER |=  PIOB_Output_mask;
    ((pio_registers_t*)PIO_PORT_B)->PIO_ODR &= ~PIOB_Output_mask;
    //------stop--------
    /* Initialize PORTB pin state */
    ((pio_registers_t*)PIO_PORT_B)->PIO_ODSR = 0x0U;
    /* PORTB drive control */
    ((pio_registers_t*)PIO_PORT_B)->PIO_DRIVER = 0x0U;

    /************************ PIO C Initialization ************************/    
    //------begin-------
    /* PORTC PIO: Default*/
    ((pio_registers_t*)PIO_PORT_C)->PIO_ABCDSR[0] = 0x0U;
    ((pio_registers_t*)PIO_PORT_C)->PIO_ABCDSR[1] = 0x0U;
    /* Select PIO_Output for PC18 */
    uint32_t PIOC_Output_mask = ( (1U << TEC_3_SW_PIN) | (1U << TEC_1_CS_PIN)| (1U << TEC_2_CS_PIN) | (1U << TEC_3_CS_PIN)
                                | (1U << TEC_4_SW_PIN) | (1U << SENSOR5_EN_PIN) | (1U << LASER_DAC_LATCH_PIN)
                                | (1U << LASER_SW_INT_CS_PIN) | (1U << SOL1_IN2_PIN) | (1U << PUMP_EN_PIN) | (1U << PSRAM_CS_PIN)
                                | (1U << FLOW_1_EN_PIN) | (1U << FLOW_2_EN_PIN));
    
    uint32_t LSM6_INT1_PIN_MASK = (1U << LSM6_INT1_PIN);
    /* Select Peripheral B for PC5, PC6, PC8, PC23 */
    uint32_t PIOC_periph_B_mask = (1U << HTR_PWM_7_PIN) | (1U << HTR_PWM_8_PIN) | (1U << HTR_PWM_5_PIN) | (1U << HTR_PWM_6_PIN);
    ((pio_registers_t*)PIO_PORT_C)->PIO_ABCDSR[0] |=  PIOC_periph_B_mask;
    ((pio_registers_t*)PIO_PORT_C)->PIO_ABCDSR[1] &= ~PIOC_periph_B_mask;
    /* Select Peripheral C for PC12, PC14 */
    uint32_t PIOC_periph_C_mask = (1U << 12) | (1U << 14) | (1U << SPI1_SCK_PIN) | (1U << SPI1_MOSI_PIN) | (1U << SPI1_MISO_PIN);
    /* Select Alternate function for pin*/
    uint32_t PIOC_ADC_mask = (1U << ADC_TEMP5_PIN) | (1U << ADC_TEMP6_PIN) | (1U << ADC_TEMP8_PIN);
    ((pio_registers_t*)PIO_PORT_C)->PIO_ABCDSR[0] &= ~PIOC_periph_C_mask;
    ((pio_registers_t*)PIO_PORT_C)->PIO_ABCDSR[1] |=  PIOC_periph_C_mask;
    //------stop--------
    ((pio_registers_t*)PIO_PORT_C)->PIO_PER = 0xFFFFFFFFU;
    //------begin-------
    /* PORTA PIO Disable and Peripheral Enable*/
    ((pio_registers_t*)PIO_PORT_C)->PIO_PDR |=  (PIOC_periph_B_mask | PIOC_periph_C_mask | PIOC_ADC_mask);
    /* PORTA PIO Enable and Peripheral Disable*/
    ((pio_registers_t*)PIO_PORT_C)->PIO_PER &= ~(PIOC_periph_B_mask | PIOC_periph_C_mask);
    //------stop--------
    /* PORTA PIO Disable Interrupt */
    ((pio_registers_t*)PIO_PORT_C)->PIO_IDR |= PIOC_ADC_mask;
    ((pio_registers_t*)PIO_PORT_C)->PIO_MDDR = 0xFFFFFFFFU;
    /* PORTC Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_C)->PIO_PUDR = 0xFFFFFFFFU;
    ((pio_registers_t*)PIO_PORT_C)->PIO_PUDR |= PIOC_ADC_mask;
    /* PORTC Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_C)->PIO_PPDDR = 0xFFFFFFFFU;
    /* PORTC Output Write Enable */
    ((pio_registers_t*)PIO_PORT_C)->PIO_OWER = PIO_OWER_Msk;
    /* PORTC Output Direction Enable */
    ((pio_registers_t*)PIO_PORT_C)->PIO_OER = 0x0U;
    ((pio_registers_t*)PIO_PORT_C)->PIO_ODR = 0xFFFFFFFFU;
    //------begin-------
    ((pio_registers_t*)PIO_PORT_C)->PIO_OER |=  PIOC_Output_mask;
    ((pio_registers_t*)PIO_PORT_C)->PIO_ODR &= ~PIOC_Output_mask;
    //------stop--------
    /* Initialize PORTC pin state */
    ((pio_registers_t*)PIO_PORT_C)->PIO_ODSR = 0x0U;
    //------begin-------
    ((pio_registers_t*)PIO_PORT_C)->PIO_ODSR |= ((1U << TEC_1_CS_PIN)| (1U << TEC_2_CS_PIN) | (1U << TEC_3_CS_PIN)
                                                |(1U << LASER_SW_INT_CS_PIN) | (1U << PSRAM_CS_PIN));
    //------stop--------
    /* PORTC Interrupt Enable */
    ((pio_registers_t*)PIO_PORT_C)->PIO_AIMER |= LSM6_INT1_PIN_MASK;
    ((pio_registers_t*)PIO_PORT_C)->PIO_ESR |= LSM6_INT1_PIN_MASK;
    ((pio_registers_t*)PIO_PORT_C)->PIO_REHLSR |= LSM6_INT1_PIN_MASK;
    ((pio_registers_t*)PIO_PORT_C)->PIO_IER |= LSM6_INT1_PIN_MASK;
    
    /* PORTC drive control */
    ((pio_registers_t*)PIO_PORT_C)->PIO_DRIVER = 0x0U;

    /************************ PIO D Initialization ************************/
    /* PORTD Peripheral Function Selection */
    //------begin-------
    /* PORTD PIO: Default*/
    ((pio_registers_t*)PIO_PORT_D)->PIO_ABCDSR[0] = 0x0U;
    ((pio_registers_t*)PIO_PORT_D)->PIO_ABCDSR[1] = 0x0U;
    /* Select PIO_Output for PD4, PD5 */
    uint32_t PIOD_Output_mask = (1U << POWER_SLN_PIN) | (1U << POWER_HD4_PIN) | (1U << POWER_TEC_PIN) | (1U << POWER_SOM_PIN) | (1U << POWER_LP_PIN)
                                | (1U << TEC_2_SW_PIN) | (1U << TEC_4_CS_PIN)
                                | (1U << PHOTO_SW_CS_PIN) | (1U << PHOTO_ADC_CS_PIN)
                                | (1U << FRAM_CS_PIN) | (1U << SW_PSRAM_SEL_PIN) 
                                | (1U << MCU_MPU_GPIO_A_PIN) | (1U << MCU_MPU_GPIO_B_PIN);

    /* Select Peripheral B for PD15, PD16, PD17 */
    uint32_t PIOD_periph_B_mask = (1U << USART2_SPI_SCK_PIN) | (1U << USART2_SPI_MOSI_PIN) | (1U << USART2_SPI_MISO_PIN) 
                                | (1U << SPI0_SCK_PIN) | (1U << SPI0_MOSI_PIN) | (1U << SPI0_MISO_PIN);
    ((pio_registers_t*)PIO_PORT_D)->PIO_ABCDSR[0] |=  PIOD_periph_B_mask;
    ((pio_registers_t*)PIO_PORT_D)->PIO_ABCDSR[1] &= ~PIOD_periph_B_mask;
    /* Select Peripheral C for PD25, PD26 */
    uint32_t PIOD_periph_C_mask = (1U << 25) | (1U << 26)| (1U << 27) | (1U << 28);
    ((pio_registers_t*)PIO_PORT_D)->PIO_ABCDSR[0] &= ~PIOD_periph_C_mask;
    ((pio_registers_t*)PIO_PORT_D)->PIO_ABCDSR[1] |=  PIOD_periph_C_mask;
    //------stop--------
    ((pio_registers_t*)PIO_PORT_D)->PIO_PER = 0x0U;
    //------begin-------
    /* PORTD PIO Enable and Peripheral Disable*/
    ((pio_registers_t*)PIO_PORT_D)->PIO_PER &= ~(PIOD_periph_B_mask | PIOD_periph_C_mask);
    /* PORTD PIO Disable and Peripheral Enable*/
    ((pio_registers_t*)PIO_PORT_D)->PIO_PDR |=  (PIOD_periph_B_mask | PIOD_periph_C_mask);
    //------stop--------
    ((pio_registers_t*)PIO_PORT_D)->PIO_MDDR = 0xFFFFFFFFU;
    /* PORTD Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_D)->PIO_PUDR = 0xFFFFFFFFU;
    /* PORTD Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_D)->PIO_PPDDR = 0xFFFFFFFFU;
    /* PORTD Output Write Enable */
    ((pio_registers_t*)PIO_PORT_D)->PIO_OWER = PIO_OWER_Msk;
    /* PORTD Output Direction Enable */
    ((pio_registers_t*)PIO_PORT_D)->PIO_OER = 0x0U;
    ((pio_registers_t*)PIO_PORT_D)->PIO_ODR = 0xFFFFFFFFU;
    //------begin-------
    ((pio_registers_t*)PIO_PORT_D)->PIO_OER |=  PIOD_Output_mask;
    ((pio_registers_t*)PIO_PORT_D)->PIO_ODR &= ~PIOD_Output_mask;
    //------stop--------
    /* Initialize PORTD pin state */
    ((pio_registers_t*)PIO_PORT_D)->PIO_ODSR = 0x0U;
    //------begin-------
    ((pio_registers_t*)PIO_PORT_D)->PIO_ODSR |= ((1U << TEC_4_CS_PIN) | (1U << PHOTO_SW_CS_PIN) | (1U << PHOTO_ADC_CS_PIN) | (1U << FRAM_CS_PIN));
    //------stop--------
    /* PORTD drive control */
    ((pio_registers_t*)PIO_PORT_D)->PIO_DRIVER = 0x0U;

    /************************ PIO E Initialization ************************/
    //------begin-------
    /* Select PIO_Output for PE1, PE2, PE3, PE4 */
    uint32_t PIOE_Output_mask = ((1U << POWER_HEATER_PIN) | (1U << POWER_PERI_PIN) | (1U << SOL2_IN2_PIN) | (1U << WDT_DONE_PIN) | (1U << WDT_WAKE_PIN));
    //------stop--------
    ((pio_registers_t*)PIO_PORT_E)->PIO_PER = 0xFFFFFFFFU;
    ((pio_registers_t*)PIO_PORT_E)->PIO_MDDR = 0xFFFFFFFFU;
    /* PORTE Pull Up Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_E)->PIO_PUDR = 0xFFFFFFFFU;
    /* PORTE Pull Down Enable/Disable as per MHC selection */
    ((pio_registers_t*)PIO_PORT_E)->PIO_PPDDR = 0xFFFFFFFFU;
    /* PORTE Output Write Enable */
    ((pio_registers_t*)PIO_PORT_E)->PIO_OWER = PIO_OWER_Msk;
    /* PORTE Output Direction Enable */
    ((pio_registers_t*)PIO_PORT_E)->PIO_OER = 0x0U;
    ((pio_registers_t*)PIO_PORT_E)->PIO_ODR = 0xFFFFFFFFU;
    //------begin-------
    ((pio_registers_t*)PIO_PORT_E)->PIO_OER |=  PIOE_Output_mask;
    ((pio_registers_t*)PIO_PORT_E)->PIO_ODR &= ~PIOE_Output_mask;
    //------stop--------
    /* Initialize PORTE pin state */
    ((pio_registers_t*)PIO_PORT_E)->PIO_ODSR = 0x0U;
    /* PORTE drive control */
    ((pio_registers_t*)PIO_PORT_E)->PIO_DRIVER = 0x0U;
}

// *****************************************************************************
// *****************************************************************************
// Section: PIO APIs which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t PIO_PortRead ( PIO_PORT port )

  Summary:
    Read all the I/O lines of the selected port.

  Description:
    This function reads the live data values on all the I/O lines of the
    selected port.  Bit values returned in each position indicate corresponding
    pin levels.
    1 = Pin is high.
    0 = Pin is low.

    This function reads the value regardless of pin configuration, whether it is
    set as as an input, driven by the PIO Controller, or driven by a peripheral.

  Remarks:
    If the port has less than 32-bits, unimplemented pins will read as
    low (0).
    Implemented pins are Right aligned in the 32-bit return value.
*/
uint32_t PIO_PortRead(PIO_PORT port)
{
    return ((pio_registers_t*)port)->PIO_PDSR;
}

// *****************************************************************************
/* Function:
    void PIO_PortWrite (PIO_PORT port, uint32_t mask, uint32_t value);

  Summary:
    Write the value on the masked I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortWrite(PIO_PORT port, uint32_t mask, uint32_t value)
{
    ((pio_registers_t*)port)->PIO_ODSR = (((pio_registers_t*)port)->PIO_ODSR & (~mask)) | (mask & value);
}

// *****************************************************************************
/* Function:
    uint32_t PIO_PortLatchRead ( PIO_PORT port )

  Summary:
    Read the latched value on all the I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
uint32_t PIO_PortLatchRead(PIO_PORT port)
{
    return (((pio_registers_t*)port)->PIO_ODSR);
}

// *****************************************************************************
/* Function:
    void PIO_PortSet ( PIO_PORT port, uint32_t mask )

  Summary:
    Set the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortSet(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_SODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortClear ( PIO_PORT port, uint32_t mask )

  Summary:
    Clear the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortClear(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_CODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortToggle ( PIO_PORT port, uint32_t mask )

  Summary:
    Toggles the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortToggle(PIO_PORT port, uint32_t mask)
{
    /* Write into Clr and Set registers */
    ((pio_registers_t*)port)->PIO_ODSR^= mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortInputEnable ( PIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as input.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortInputEnable(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_ODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortOutputEnable ( PIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as output(s).

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortOutputEnable(PIO_PORT port, uint32_t mask)
{
    ((pio_registers_t*)port)->PIO_OER = mask;
}








/*******************************************************************************
 End of File
*/