/*******************************************************************************
  USART2 SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart2_spi.c

  Summary:
    USART2 SPI PLIB Implementation File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

#include "device.h"
#include "plib_usart2_spi.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: USART2 SPI Implementation
// *****************************************************************************
// *****************************************************************************
static volatile USART_SPI_OBJECT usart2SPIObj;


void USART2_SPI_Initialize( void )
{
    /* Configure USART2 mode to SPI Master (0x0E) */
    USART2_REGS->US_MR = US_MR_SPI_USART_MODE(US_MR_SPI_USART_MODE_SPI_MASTER_Val);

    /* Reset SPI RX, SPI TX and SPI status */
    USART2_REGS->US_CR = (US_CR_SPI_RSTRX_Msk | US_CR_SPI_RSTTX_Msk | US_CR_SPI_RSTSTA_Msk);

    /* Configure clock source, clock phase, clock polarity and CKO = 1 */
    USART2_REGS->US_MR |= (US_MR_USART_USCLKS_MCK | US_MR_SPI_CHRL(US_MR_SPI_CHRL_8_BIT_Val) | US_MR_SPI_CPHA(0x0U) | US_MR_SPI_CPOL(0x1U) | US_MR_SPI_CLKO(1U));

    /* Enable TX and RX */
    USART2_REGS->US_CR = (US_CR_SPI_RXEN_Msk | US_CR_SPI_TXEN_Msk);

    /* Configure USART2 Baud Rate */
    USART2_REGS->US_BRGR = US_BRGR_CD(120U);

    /* Initialize instance object */
    usart2SPIObj.callback = NULL;
    usart2SPIObj.context = 0U;
    usart2SPIObj.transferIsBusy = false;
}

bool USART2_SPI_TransferSetup( USART_SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock )
{
    uint32_t clockDivider = 0;
    bool setupStatus = false;

    if ((setup != NULL) && (setup->clockFrequency != 0U))
    {
        if(spiSourceClock == 0U)
        {
            // Fetch Master Clock Frequency directly
            spiSourceClock = 120000000UL;
        }

        clockDivider = spiSourceClock/setup->clockFrequency;

        if(clockDivider < 6U)
        {
            clockDivider = 6U;
        }
        else if(clockDivider > 65535U)
        {
            clockDivider = 65535U;
        }
        else
        {
           /* Clock divider is valid */
        }

        USART2_REGS->US_MR = ((USART2_REGS->US_MR & ~US_MR_SPI_CPOL_Msk) | (uint32_t)setup->clockPolarity);
        USART2_REGS->US_MR = ((USART2_REGS->US_MR & ~US_MR_SPI_CPHA_Msk) | (uint32_t)setup->clockPhase);
        USART2_REGS->US_MR = ((USART2_REGS->US_MR & ~US_MR_SPI_CHRL_Msk) | (uint32_t)setup->dataBits);

        USART2_REGS->US_BRGR = ((USART2_REGS->US_BRGR & ~US_BRGR_CD_Msk) | US_BRGR_CD(clockDivider));
        setupStatus = true;
    }
    return setupStatus;
}

bool USART2_SPI_WriteRead( void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize )
{
    bool isRequestAccepted = false;
    uint32_t dummyData;

    if (usart2SPIObj.transferIsBusy == false)
    {
        /* Verify the request */
        if(((txSize > 0U) && (pTransmitData != NULL)) || ((rxSize > 0U) && (pReceiveData != NULL)))
        {
            isRequestAccepted = true;

            usart2SPIObj.transferIsBusy = true;
            usart2SPIObj.txBuffer = pTransmitData;
            usart2SPIObj.rxBuffer = pReceiveData;
            usart2SPIObj.rxCount = 0;
            usart2SPIObj.txCount = 0;
            usart2SPIObj.dummySize = 0;

            if (pTransmitData != NULL)
            {
                usart2SPIObj.txSize = txSize;
            }
            else
            {
                usart2SPIObj.txSize = 0;
            }

            if (pReceiveData != NULL)
            {
                usart2SPIObj.rxSize = rxSize;
            }
            else
            {
                usart2SPIObj.rxSize = 0;
            }

            /* Reset over-run error if any */
            USART2_REGS->US_CR = US_CR_SPI_RSTSTA_Msk;

            /* Flush out any unread data in SPI read buffer */
            if ((USART2_REGS->US_CSR & US_CSR_SPI_RXRDY_Msk) != 0U)
            {
                dummyData = USART2_REGS->US_RHR;
                (void)dummyData;
            }

            size_t txSz = usart2SPIObj.txSize;

            if (usart2SPIObj.rxSize > txSz)
            {
                usart2SPIObj.dummySize = usart2SPIObj.rxSize - txSz;
            }

            /* Start the first write here itself, rest will happen in ISR context */
            if ((USART2_REGS->US_MR & US_MR_SPI_CHRL_Msk) == US_MR_SPI_CHRL_8_BIT)
            {
                /* Force Chip Select */
                USART2_REGS->US_CR = US_CR_SPI_FCS_Msk;

                if (usart2SPIObj.txCount < txSz)
                {
                    USART2_REGS->US_THR = *((uint8_t*)usart2SPIObj.txBuffer);
                    usart2SPIObj.txCount++;
                }
                else if (usart2SPIObj.dummySize > 0U)
                {
                    USART2_REGS->US_THR = (uint8_t)(0xff);
                    usart2SPIObj.dummySize--;
                }
                else
                {
                    /* Do nothing */
                }
            }

            if (rxSize > 0U)
            {
                /* Enable receive interrupt to complete the transfer in ISR context */
                USART2_REGS->US_IER = US_IER_SPI_RXRDY_Msk;
            }
            else
            {
                /* Enable transmit interrupt to complete the transfer in ISR context */
                USART2_REGS->US_IER = US_IER_SPI_TXRDY_Msk;
            }
        }
    }

    return isRequestAccepted;
}

bool USART2_SPI_Write( void* pTransmitData, size_t txSize )
{
    return(USART2_SPI_WriteRead(pTransmitData, txSize, NULL, 0U));
}

bool USART2_SPI_Read( void* pReceiveData, size_t rxSize )
{
    return(USART2_SPI_WriteRead(NULL, 0U, pReceiveData, rxSize));
}

bool USART2_SPI_IsTransmitterBusy(void)
{
    return ((USART2_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk) == 0U)? true : false;
}

bool USART2_SPI_IsBusy( void )
{
    bool transferIsBusy = usart2SPIObj.transferIsBusy;

    return (((USART2_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk) == 0U) || (transferIsBusy));
}

void USART2_SPI_CallbackRegister( USART_SPI_CALLBACK callback, uintptr_t context )
{
    usart2SPIObj.callback = callback;
    usart2SPIObj.context = context;
}

void __attribute__((used)) USART2_InterruptHandler( void )
{
    uint32_t receivedData;


    size_t rxCount = usart2SPIObj.rxCount;

    if ((USART2_REGS->US_CSR & US_CSR_SPI_RXRDY_Msk) != 0U)
    {
        receivedData = USART2_REGS->US_RHR;

        if (rxCount < usart2SPIObj.rxSize)
        {
            ((uint8_t*)usart2SPIObj.rxBuffer)[rxCount] = (uint8_t)receivedData;
            rxCount++;
        }

        usart2SPIObj.rxCount = rxCount;
    }

    if((USART2_REGS->US_CSR & US_CSR_SPI_TXRDY_Msk) != 0U)
    {
        /* Disable the TXRDY interrupt. This will be enabled back if one or more
         * bytes are pending transmission */

        USART2_REGS->US_IDR = US_IDR_SPI_TXRDY_Msk;

        size_t txCount = usart2SPIObj.txCount;
        size_t txSize = usart2SPIObj.txSize;

        if (txCount < usart2SPIObj.txSize)
        {
            USART2_REGS->US_THR = ((uint8_t*)usart2SPIObj.txBuffer)[txCount];
            txCount++;
        }
        else if (usart2SPIObj.dummySize > 0U)
        {
            USART2_REGS->US_THR = (uint8_t)(0xff);
            usart2SPIObj.dummySize--;
        }
        else
        {
            /* Do nothing */
        }

        usart2SPIObj.txCount = txCount;

        if ((usart2SPIObj.dummySize == 0U) && (txCount == txSize))
        {
            if ((USART2_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk) != 0U)
            {
                /* Disable all interrupt sources - RXRDY, TXRDY and TXEMPTY */
                USART2_REGS->US_IDR = (US_IDR_SPI_RXRDY_Msk | US_IDR_SPI_TXRDY_Msk | US_IDR_SPI_TXEMPTY_Msk);

                usart2SPIObj.transferIsBusy = false;

                /* Release Chip Select */
                USART2_REGS->US_CR = US_CR_SPI_RCS_Msk;

                /* All characters are transmitted out and TX shift register is empty */
                if(usart2SPIObj.callback != NULL)
                {
                    uintptr_t context = usart2SPIObj.context;

                    usart2SPIObj.callback(context);
                }
            }
            else
            {
                /* Enable TXEMPTY interrupt */
                USART2_REGS->US_IER = US_IER_SPI_TXEMPTY_Msk;
            }
        }
        else if (rxCount == usart2SPIObj.rxSize)
        {
            /* Enable TXRDY interrupt as all the requested bytes are received
             * and can now make use of the internal transmit shift register.
             */
            USART2_REGS->US_IDR = US_IDR_SPI_RXRDY_Msk;
            USART2_REGS->US_IER = US_IER_SPI_TXRDY_Msk;
        }
        else
        {
            /* Do nothing */
        }
    }
}
