/*
 * fram_usart_spi.h
 *
 *  Created on: Jan 08, 2026
 */

#ifndef FRAM_USART_SPI_H_
#define FRAM_USART_SPI_H_

#include "definitions.h"

#include "peripheral/usart/plib_usart0_spi.h"

#include "device.h"
#include <stdint.h>
#include <stdbool.h>

/* Return status definitions */
typedef enum {
    FRAM_OK = 0,
    FRAM_ERROR = 1,
    FRAM_BUSY = 2,
    FRAM_TIMEOUT = 3
} FRAM_StatusTypeDef;

/* FRAM operating mode */
typedef enum {
    FRAM_MODE_POLLING = 0,
    FRAM_MODE_INTERRUPT = 1
} FRAM_ModeTypeDef;

/* FRAM handle structure */
typedef struct {
    FRAM_ModeTypeDef mode;          /* Operating mode: polling or interrupt */
    volatile bool transferComplete;  /* Transfer completion flag for interrupt mode */
    volatile FRAM_StatusTypeDef lastStatus; /* Last operation status */
} FRAM_USART_HandleTypeDef;

/* Callback function type */
typedef void (*FRAM_CallbackTypeDef)(FRAM_StatusTypeDef status);

/**
 * @brief Get FRAM handle instance
 * @return Pointer to the FRAM handle
 */
FRAM_USART_HandleTypeDef* FRAM_USART_GetHandle(void);

/**
 * @brief Initialize the FRAM driver
 * @param mode: Operating mode (FRAM_MODE_POLLING or FRAM_MODE_INTERRUPT)
 * @return Status of initialization
 */
FRAM_StatusTypeDef FRAM_USART_Init(FRAM_ModeTypeDef mode);

/**
 * @brief Register callback function for interrupt mode
 * @param callback: Pointer to callback function
 */
void FRAM_USART_RegisterCallback(FRAM_CallbackTypeDef callback);

/* ==================== POLLING MODE FUNCTIONS ==================== */

/**
 * @brief Send Write Enable command (Polling)
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_WriteEnable_Polling(void);

/**
 * @brief Send Write Disable command (Polling)
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_WriteDisable_Polling(void);

/**
 * @brief Write data to FRAM memory (Polling)
 * @param addr: 24-bit memory address
 * @param pData: Pointer to data buffer
 * @param len: Number of bytes to write
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_WriteMem_Polling(uint32_t addr, uint8_t *pData, uint16_t len);

/**
 * @brief Read data from FRAM memory (Polling)
 * @param addr: 24-bit memory address
 * @param pData: Pointer to receive buffer
 * @param len: Number of bytes to read
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_ReadMem_Polling(uint32_t addr, uint8_t *pData, uint16_t len);

/**
 * @brief Read Device ID (Polling)
 * @param pID: Pointer to ID buffer
 * @param len: Number of bytes to read
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_ReadID_Polling(uint8_t *pID, uint16_t len);

/* ==================== INTERRUPT MODE FUNCTIONS ==================== */

/**
 * @brief Send Write Enable command (Interrupt)
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_WriteEnable_IT(void);

/**
 * @brief Send Write Disable command (Interrupt)
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_WriteDisable_IT(void);

/**
 * @brief Write data to FRAM memory (Interrupt)
 * @param addr: 24-bit memory address
 * @param pData: Pointer to data buffer
 * @param len: Number of bytes to write
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_WriteMem_IT(uint32_t addr, uint8_t *pData, uint16_t len);

/**
 * @brief Read data from FRAM memory (Interrupt)
 * @param addr: 24-bit memory address
 * @param pData: Pointer to receive buffer
 * @param len: Number of bytes to read
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_ReadMem_IT(uint32_t addr, uint8_t *pData, uint16_t len);

/**
 * @brief Read Device ID (Interrupt)
 * @param pID: Pointer to ID buffer
 * @param len: Number of bytes to read
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_ReadID_IT(uint8_t *pID, uint16_t len);

/**
 * @brief Wait for interrupt transfer to complete
 * @param timeout_ms: Timeout in milliseconds
 * @return Operation status
 */
FRAM_StatusTypeDef FRAM_USART_WaitForComplete(uint32_t timeout_ms);

/* CS (Chip Select) pin control macros */
#define FRAM_CS_LOW()    GPIO_PD28_Clear()
#define FRAM_CS_HIGH()   GPIO_PD28_Set()

/* FRAM Command Definitions */
#define FRAM_CMD_WREN    0x06   /* Write Enable */
#define FRAM_CMD_WRDI    0x04   /* Write Disable */
#define FRAM_CMD_RDSR    0x05   /* Read Status Register */
#define FRAM_CMD_WRSR    0x01   /* Write Status Register */
#define FRAM_CMD_READ    0x03   /* Read Memory */
#define FRAM_CMD_WRITE   0x02   /* Write Memory */
#define FRAM_CMD_RDID    0x9F   /* Read Device ID */
#define FRAM_CMD_FSTRD   0x0B   /* Fast Read */
#define FRAM_CMD_SLEEP   0xB9   /* Sleep Mode */

#endif /* FRAM_USART_SPI_H_ */