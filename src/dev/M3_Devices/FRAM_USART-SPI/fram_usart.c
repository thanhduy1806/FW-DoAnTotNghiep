/*
 * fram_usart_spi.c
 *
 *  Created on: Jan, 2026
 */

#include "fram_usart.h"
#include <string.h>


/* FRAM handle instance */
static FRAM_USART_HandleTypeDef hfram;

/* Callback function pointer */
static FRAM_CallbackTypeDef fram_callback = NULL;

/* Timeout counter (used with systick or timer) */
#define FRAM_TIMEOUT_DEFAULT    1000

/* Get FRAM handle */
FRAM_USART_HandleTypeDef* FRAM_USART_GetHandle(void)
{
    return &hfram;
}

/* USART SPI callback for interrupt mode */
static void FRAM_USART_SPI_Callback(uintptr_t context)
{
    hfram.transferComplete = true;
    hfram.lastStatus = FRAM_OK;
    
    if (fram_callback != NULL) {
        fram_callback(FRAM_OK);
    }
}

/* Initialize FRAM driver */
FRAM_StatusTypeDef FRAM_USART_Init(FRAM_ModeTypeDef mode)
{
    /* Initialize USART0 in SPI mode */
    // USART0_SPI_Initialize();
    
    /* Configure CS pin as output and set high (inactive) */
    // GPIO_PD0_OutputEnable();
    FRAM_CS_HIGH();
    
    /* Initialize handle */
    hfram.mode = mode;
    hfram.transferComplete = false;
    hfram.lastStatus = FRAM_OK;
    
    /* Register callback if interrupt mode */
    if (mode == FRAM_MODE_INTERRUPT) {
        USART0_SPI_CallbackRegister(FRAM_USART_SPI_Callback, 0);
    }
    
    return FRAM_OK;
}

/* Register callback function */
void FRAM_USART_RegisterCallback(FRAM_CallbackTypeDef callback)
{
    fram_callback = callback;
}

/* ==================== POLLING MODE FUNCTIONS ==================== */

/* Write Enable - Polling */
FRAM_StatusTypeDef FRAM_USART_WriteEnable_Polling(void)
{
    uint8_t cmd = FRAM_CMD_WREN;
    
    FRAM_CS_LOW();
    
    if (!USART0_SPI_Write(&cmd, 1)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    /* Wait for transmission complete */
    while (USART0_SPI_IsBusy());
    
    FRAM_CS_HIGH();
    
    return FRAM_OK;
}

/* Write Disable - Polling */
FRAM_StatusTypeDef FRAM_USART_WriteDisable_Polling(void)
{
    uint8_t cmd = FRAM_CMD_WRDI;
    
    FRAM_CS_LOW();
    
    if (!USART0_SPI_Write(&cmd, 1)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    /* Wait for transmission complete */
    while (USART0_SPI_IsBusy());
    
    FRAM_CS_HIGH();
    
    return FRAM_OK;
}

/* Write Memory - Polling */
FRAM_StatusTypeDef FRAM_USART_WriteMem_Polling(uint32_t addr, uint8_t *pData, uint16_t len)
{
    uint8_t txBuffer[4];
    
    if (pData == NULL || len == 0) {
        return FRAM_ERROR;
    }
    
    /* Send Write Enable command */
    if (FRAM_USART_WriteEnable_Polling() != FRAM_OK) {
        return FRAM_ERROR;
    }
    
    /* Prepare command and address */
    txBuffer[0] = FRAM_CMD_WRITE;
    txBuffer[1] = (uint8_t)((addr >> 16) & 0xFF);
    txBuffer[2] = (uint8_t)((addr >> 8) & 0xFF);
    txBuffer[3] = (uint8_t)(addr & 0xFF);
    
    FRAM_CS_LOW();
    
    /* Send command and address */
    if (!USART0_SPI_Write(txBuffer, 4)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    /* Wait for command/address transmission complete */
    while (USART0_SPI_IsBusy());
    
    /* Send data */
    if (!USART0_SPI_Write(pData, len)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    /* Wait for data transmission complete */
    while (USART0_SPI_IsBusy());
    
    FRAM_CS_HIGH();
    
    /* Send Write Disable command */
    FRAM_USART_WriteDisable_Polling();
    
    return FRAM_OK;
}

/* Read Memory - Polling */
FRAM_StatusTypeDef FRAM_USART_ReadMem_Polling(uint32_t addr, uint8_t *pData, uint16_t len)
{
    uint8_t txBuffer[4];
    
    if (pData == NULL || len == 0) {
        return FRAM_ERROR;
    }
    
    /* Prepare command and address */
    txBuffer[0] = FRAM_CMD_READ;
    txBuffer[1] = (uint8_t)((addr >> 16) & 0xFF);
    txBuffer[2] = (uint8_t)((addr >> 8) & 0xFF);
    txBuffer[3] = (uint8_t)(addr & 0xFF);
    
    FRAM_CS_LOW();
    
    /* Send command and address */
    if (!USART0_SPI_Write(txBuffer, 4)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    /* Wait for command/address transmission complete */
    while (USART0_SPI_IsBusy());
    
    /* Read data */
    if (!USART0_SPI_Read(pData, len)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    /* Wait for read complete */
    while (USART0_SPI_IsBusy());
    
    FRAM_CS_HIGH();
    
    return FRAM_OK;
}

/* Read Device ID - Polling */
FRAM_StatusTypeDef FRAM_USART_ReadID_Polling(uint8_t *pID, uint16_t len)
{
    uint8_t cmd = FRAM_CMD_RDID;
    
    if (pID == NULL || len == 0) {
        return FRAM_ERROR;
    }
    
    FRAM_CS_LOW();
    
    /* Send command */
    if (!USART0_SPI_Write(&cmd, 1)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    /* Wait for command transmission complete */
    while (USART0_SPI_IsBusy());
    
    /* Read ID */
    if (!USART0_SPI_Read(pID, len)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    /* Wait for read complete */
    while (USART0_SPI_IsBusy());
    
    FRAM_CS_HIGH();
    
    return FRAM_OK;
}

/* ==================== INTERRUPT MODE FUNCTIONS ==================== */

/* Write Enable - Interrupt */
FRAM_StatusTypeDef FRAM_USART_WriteEnable_IT(void)
{
    uint8_t cmd = FRAM_CMD_WREN;
    
    if (USART0_SPI_IsBusy()) {
        return FRAM_BUSY;
    }
    
    hfram.transferComplete = false;
    
    FRAM_CS_LOW();
    
    if (!USART0_SPI_Write(&cmd, 1)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    return FRAM_OK;
}

/* Write Disable - Interrupt */
FRAM_StatusTypeDef FRAM_USART_WriteDisable_IT(void)
{
    uint8_t cmd = FRAM_CMD_WRDI;
    
    if (USART0_SPI_IsBusy()) {
        return FRAM_BUSY;
    }
    
    hfram.transferComplete = false;
    
    FRAM_CS_LOW();
    
    if (!USART0_SPI_Write(&cmd, 1)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    return FRAM_OK;
}

/* Write Memory - Interrupt */
FRAM_StatusTypeDef FRAM_USART_WriteMem_IT(uint32_t addr, uint8_t *pData, uint16_t len)
{
    static uint8_t txBuffer[4 + 256]; /* Command + Address + Data buffer */
    
    if (pData == NULL || len == 0 || len > 252) {
        return FRAM_ERROR;
    }
    
    if (USART0_SPI_IsBusy()) {
        return FRAM_BUSY;
    }
    
    /* Send Write Enable first (blocking) */
    if (FRAM_USART_WriteEnable_Polling() != FRAM_OK) {
        return FRAM_ERROR;
    }
    
    /* Prepare command, address and data in one buffer */
    txBuffer[0] = FRAM_CMD_WRITE;
    txBuffer[1] = (uint8_t)((addr >> 16) & 0xFF);
    txBuffer[2] = (uint8_t)((addr >> 8) & 0xFF);
    txBuffer[3] = (uint8_t)(addr & 0xFF);
    memcpy(&txBuffer[4], pData, len);
    
    hfram.transferComplete = false;
    
    FRAM_CS_LOW();
    
    /* Send all at once in interrupt mode */
    if (!USART0_SPI_Write(txBuffer, 4 + len)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    return FRAM_OK;
}

/* Read Memory - Interrupt */
FRAM_StatusTypeDef FRAM_USART_ReadMem_IT(uint32_t addr, uint8_t *pData, uint16_t len)
{
    static uint8_t txBuffer[4];
    
    if (pData == NULL || len == 0) {
        return FRAM_ERROR;
    }
    
    if (USART0_SPI_IsBusy()) {
        return FRAM_BUSY;
    }
    
    /* Prepare command and address */
    txBuffer[0] = FRAM_CMD_READ;
    txBuffer[1] = (uint8_t)((addr >> 16) & 0xFF);
    txBuffer[2] = (uint8_t)((addr >> 8) & 0xFF);
    txBuffer[3] = (uint8_t)(addr & 0xFF);
    
    hfram.transferComplete = false;
    
    FRAM_CS_LOW();
    
    /* Send command/address and read data in one transaction */
    if (!USART0_SPI_WriteRead(txBuffer, 4, pData, len)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    return FRAM_OK;
}

/* Read Device ID - Interrupt */
FRAM_StatusTypeDef FRAM_USART_ReadID_IT(uint8_t *pID, uint16_t len)
{
    static uint8_t cmd = FRAM_CMD_RDID;
    
    if (pID == NULL || len == 0) {
        return FRAM_ERROR;
    }
    
    if (USART0_SPI_IsBusy()) {
        return FRAM_BUSY;
    }
    
    hfram.transferComplete = false;
    
    FRAM_CS_LOW();
    
    /* Send command and read ID in one transaction */
    if (!USART0_SPI_WriteRead(&cmd, 1, pID, len)) {
        FRAM_CS_HIGH();
        return FRAM_ERROR;
    }
    
    return FRAM_OK;
}

/* Wait for interrupt transfer to complete */
FRAM_StatusTypeDef FRAM_USART_WaitForComplete(uint32_t timeout_ms)
{
    uint32_t timeout = timeout_ms;
    
    while (!hfram.transferComplete && timeout > 0) {
        /* Add delay here based on your system tick (e.g., 1ms delay) */
        /* For example: vTaskDelay(1) or HAL_Delay(1) */
        timeout--;
    }
    
    FRAM_CS_HIGH(); /* Release CS after complete */
    
    if (timeout == 0) {
        return FRAM_TIMEOUT;
    }
    
    return hfram.lastStatus;
}