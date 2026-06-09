/*******************************************************************************
  USART0 SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_usart0_spi.c

  Summary:
    USART0 SPI PLIB Implementation File

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
#include "plib_usart0_spi.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: USART0 SPI Implementation
// *****************************************************************************
// *****************************************************************************
static volatile USART_SPI_OBJECT usart0SPIObj;


void USART0_SPI_Initialize( void )
{
    /* Configure USART0 mode to SPI Master (0x0E) */
    USART0_REGS->US_MR = US_MR_SPI_USART_MODE(US_MR_SPI_USART_MODE_SPI_MASTER_Val);

    /* Reset SPI RX, SPI TX and SPI status */
    USART0_REGS->US_CR = (US_CR_SPI_RSTRX_Msk | US_CR_SPI_RSTTX_Msk | US_CR_SPI_RSTSTA_Msk);

    /* Configure clock source, clock phase, clock polarity and CKO = 1 */
    USART0_REGS->US_MR |= (US_MR_USART_USCLKS_MCK | US_MR_SPI_CHRL(US_MR_SPI_CHRL_8_BIT_Val) | US_MR_SPI_CPHA(0x0U) | US_MR_SPI_CPOL(0x0U) | US_MR_SPI_CLKO(1U));

    /* Enable TX and RX */
    USART0_REGS->US_CR = (US_CR_SPI_RXEN_Msk | US_CR_SPI_TXEN_Msk);

    /* Configure USART0 Baud Rate */
    USART0_REGS->US_BRGR = US_BRGR_CD(60U);

    /* Initialize instance object */
    usart0SPIObj.callback = NULL;
    usart0SPIObj.context = 0U;
    usart0SPIObj.transferIsBusy = false;
}

bool USART0_SPI_TransferSetup( USART_SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock )
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

        USART0_REGS->US_MR = ((USART0_REGS->US_MR & ~US_MR_SPI_CPOL_Msk) | (uint32_t)setup->clockPolarity);
        USART0_REGS->US_MR = ((USART0_REGS->US_MR & ~US_MR_SPI_CPHA_Msk) | (uint32_t)setup->clockPhase);
        USART0_REGS->US_MR = ((USART0_REGS->US_MR & ~US_MR_SPI_CHRL_Msk) | (uint32_t)setup->dataBits);

        USART0_REGS->US_BRGR = ((USART0_REGS->US_BRGR & ~US_BRGR_CD_Msk) | US_BRGR_CD(clockDivider));
        setupStatus = true;
    }
    return setupStatus;
}

bool USART0_SPI_WriteRead( void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize )
{
    bool isRequestAccepted = false;
    uint32_t dummyData;

    if (usart0SPIObj.transferIsBusy == false)
    {
        /* Verify the request */
        if(((txSize > 0U) && (pTransmitData != NULL)) || ((rxSize > 0U) && (pReceiveData != NULL)))
        {
            isRequestAccepted = true;

            usart0SPIObj.transferIsBusy = true;
            usart0SPIObj.txBuffer = pTransmitData;
            usart0SPIObj.rxBuffer = pReceiveData;
            usart0SPIObj.rxCount = 0;
            usart0SPIObj.txCount = 0;
            usart0SPIObj.dummySize = 0;

            if (pTransmitData != NULL)
            {
                usart0SPIObj.txSize = txSize;
            }
            else
            {
                usart0SPIObj.txSize = 0;
            }

            if (pReceiveData != NULL)
            {
                usart0SPIObj.rxSize = rxSize;
            }
            else
            {
                usart0SPIObj.rxSize = 0;
            }

            /* Reset over-run error if any */
            USART0_REGS->US_CR = US_CR_SPI_RSTSTA_Msk;

            /* Flush out any unread data in SPI read buffer */
            if ((USART0_REGS->US_CSR & US_CSR_SPI_RXRDY_Msk) != 0U)
            {
                dummyData = USART0_REGS->US_RHR;
                (void)dummyData;
            }

            size_t txSz = usart0SPIObj.txSize;

            if (usart0SPIObj.rxSize > txSz)
            {
                usart0SPIObj.dummySize = usart0SPIObj.rxSize - txSz;
            }

            /* Start the first write here itself, rest will happen in ISR context */
            if ((USART0_REGS->US_MR & US_MR_SPI_CHRL_Msk) == US_MR_SPI_CHRL_8_BIT)
            {

                if (usart0SPIObj.txCount < txSz)
                {
                    USART0_REGS->US_THR = *((uint8_t*)usart0SPIObj.txBuffer);
                    usart0SPIObj.txCount++;
                }
                else if (usart0SPIObj.dummySize > 0U)
                {
                    USART0_REGS->US_THR = (uint8_t)(0xff);
                    usart0SPIObj.dummySize--;
                }
                else
                {
                    /* Do nothing */
                }
            }

            if (rxSize > 0U)
            {
                /* Enable receive interrupt to complete the transfer in ISR context */
                USART0_REGS->US_IER = US_IER_SPI_RXRDY_Msk;
            }
            else
            {
                /* Enable transmit interrupt to complete the transfer in ISR context */
                USART0_REGS->US_IER = US_IER_SPI_TXRDY_Msk;
            }
        }
    }

    return isRequestAccepted;
}

bool USART0_SPI_Write( void* pTransmitData, size_t txSize )
{
    return(USART0_SPI_WriteRead(pTransmitData, txSize, NULL, 0U));
}

bool USART0_SPI_Read( void* pReceiveData, size_t rxSize )
{
    return(USART0_SPI_WriteRead(NULL, 0U, pReceiveData, rxSize));
}

bool USART0_SPI_IsTransmitterBusy(void)
{
    return ((USART0_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk) == 0U)? true : false;
}

bool USART0_SPI_IsBusy( void )
{
    bool transferIsBusy = usart0SPIObj.transferIsBusy;

    return (((USART0_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk) == 0U) || (transferIsBusy));
}

void USART0_SPI_CallbackRegister( USART_SPI_CALLBACK callback, uintptr_t context )
{
    usart0SPIObj.callback = callback;
    usart0SPIObj.context = context;
}

void __attribute__((used)) USART0_InterruptHandler( void )
{
    uint32_t receivedData;


    size_t rxCount = usart0SPIObj.rxCount;

    if ((USART0_REGS->US_CSR & US_CSR_SPI_RXRDY_Msk) != 0U)
    {
        receivedData = USART0_REGS->US_RHR;

        if (rxCount < usart0SPIObj.rxSize)
        {
            ((uint8_t*)usart0SPIObj.rxBuffer)[rxCount] = (uint8_t)receivedData;
            rxCount++;
        }

        usart0SPIObj.rxCount = rxCount;
    }

    if((USART0_REGS->US_CSR & US_CSR_SPI_TXRDY_Msk) != 0U)
    {
        /* Disable the TXRDY interrupt. This will be enabled back if one or more
         * bytes are pending transmission */

        USART0_REGS->US_IDR = US_IDR_SPI_TXRDY_Msk;

        size_t txCount = usart0SPIObj.txCount;
        size_t txSize = usart0SPIObj.txSize;

        if (txCount < usart0SPIObj.txSize)
        {
            USART0_REGS->US_THR = ((uint8_t*)usart0SPIObj.txBuffer)[txCount];
            txCount++;
        }
        else if (usart0SPIObj.dummySize > 0U)
        {
            USART0_REGS->US_THR = (uint8_t)(0xff);
            usart0SPIObj.dummySize--;
        }
        else
        {
            /* Do nothing */
        }

        usart0SPIObj.txCount = txCount;

        if ((usart0SPIObj.dummySize == 0U) && (txCount == txSize))
        {
            if ((USART0_REGS->US_CSR & US_CSR_SPI_TXEMPTY_Msk) != 0U)
            {
                /* Disable all interrupt sources - RXRDY, TXRDY and TXEMPTY */
                USART0_REGS->US_IDR = (US_IDR_SPI_RXRDY_Msk | US_IDR_SPI_TXRDY_Msk | US_IDR_SPI_TXEMPTY_Msk);

                usart0SPIObj.transferIsBusy = false;


                /* All characters are transmitted out and TX shift register is empty */
                if(usart0SPIObj.callback != NULL)
                {
                    uintptr_t context = usart0SPIObj.context;

                    usart0SPIObj.callback(context);
                }
            }
            else
            {
                /* Enable TXEMPTY interrupt */
                USART0_REGS->US_IER = US_IER_SPI_TXEMPTY_Msk;
            }
        }
        else if (rxCount == usart0SPIObj.rxSize)
        {
            /* Enable TXRDY interrupt as all the requested bytes are received
             * and can now make use of the internal transmit shift register.
             */
            USART0_REGS->US_IDR = US_IDR_SPI_RXRDY_Msk;
            USART0_REGS->US_IER = US_IER_SPI_TXRDY_Msk;
        }
        else
        {
            /* Do nothing */
        }
    }
}
