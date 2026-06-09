/*
 * spi_io.h
 *
 *  Created on: Aug 3, 2025
 *      Author: Admin
 *
 *  Notes:
 *  - OS abstraction uses opaque lock handle (os_lock_t) from os.h
 *  - No direct FreeRTOS/CMSIS-RTOS types appear in this header
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "os_hal.h"
#include "define.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SPI_CPHA        (0x01u)
#define SPI_CPOL        (0x02u)

#ifndef SPI_WAIT_TIME
#define SPI_WAIT_TIME   (250u)     /* default timeout in ms (if used by implementation) */
#endif

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum spi_mode
 * @brief SPI configuration for clock phase and polarity.
 */
typedef enum
{
    /** CPOL=0, CPHA=0: sample on rising, shift on falling */
    SPI_MODE_0 = (0u | 0u),
    /** CPOL=0, CPHA=1: sample on falling, shift on rising */
    SPI_MODE_1 = (0u | SPI_CPHA),
    /** CPOL=1, CPHA=0: sample on rising, shift on falling (clock idle high) */
    SPI_MODE_2 = (SPI_CPOL | 0u),
    /** CPOL=1, CPHA=1: sample on falling, shift on rising (clock idle high) */
    SPI_MODE_3 = (SPI_CPOL | SPI_CPHA)
} spi_mode_t;

/**
 * @enum spi_bit_order
 * @brief SPI configuration for bit order.
 */
typedef enum
{
    SPI_BIT_ORDER_MSB_FIRST = 0u,
    SPI_BIT_ORDER_LSB_FIRST = 1u
} spi_bit_order_t;

/**
 * @enum spi_lanes
 * @brief SPI number of lanes.
 */
typedef enum
{
    SPI_SINGLE_LANE = 0u,
    SPI_DUAL_LANE   = 1u,
    SPI_QUAD_LANE   = 2u,
    SPI_OCTO_LANE   = 3u
} spi_lanes_t;

/**
 * @struct spi_io_t
 * @brief SPI IO device context (port + OS lock).
 *
 * ui32SpiPort: user-defined SPI instance index/ID (e.g., SERCOM number, SPIx base, etc.)
 * lock: protects exclusive access to the SPI bus from multiple tasks.
 */
typedef struct
{
    uint32_t   ui32SpiPort;
    os_lock_t  lock;
} spi_io_t;

/******************************************************************************/
/*************************** Public API Declarations **************************/
/******************************************************************************/

/* Configuration */
uint32_t spi_io_init(spi_io_t *me);
uint32_t spi_io_set_frame_size(spi_io_t *me, uint8_t bits);
uint32_t spi_io_set_mode(spi_io_t *me, uint8_t spi_mode);
uint32_t spi_io_set_prescale(spi_io_t *me, uint8_t scbr);

/* Synchronous (blocking) */
uint32_t spi_io_read_sync(spi_io_t *me, uint8_t *pui8RxBuff, uint32_t ui32Length);
uint32_t spi_io_write_sync(spi_io_t *me, const uint8_t *pui8TxBuff, uint32_t ui32Length);
uint32_t spi_io_transfer_sync(spi_io_t *me, const uint8_t *pui8TxBuff, uint8_t *pui8RxBuff, uint32_t ui32Length);
uint32_t spi_io_write_and_read_sync(spi_io_t *me,
                                    const uint8_t *pui8TxBuff, uint32_t ui32TxLength,
                                    uint8_t *pui8RxBuff, uint32_t ui32RxLength);

/* Asynchronous (interrupt/callback driven) */
uint32_t spi_io_read_async(spi_io_t *me, uint8_t *pui8RxBuff, uint32_t ui32Length);
uint32_t spi_io_write_async(spi_io_t *me, const uint8_t *pui8TxBuff, uint32_t ui32Length);
uint32_t spi_io_transfer_async(spi_io_t *me, const uint8_t *pui8TxBuff, uint8_t *pui8RxBuff, uint32_t ui32Length);
uint32_t spi_io_write_and_read_async(spi_io_t *me,
                                     const uint8_t *pui8TxBuff, uint32_t ui32TxLength,
                                     uint8_t *pui8RxBuff, uint32_t ui32RxLength);

/* DMA */
uint32_t spi_io_read_dma(spi_io_t *me, uint8_t *pui8RxBuff, uint32_t ui32Length);
uint32_t spi_io_write_dma(spi_io_t *me, const uint8_t *pui8TxBuff, uint32_t ui32Length);
uint32_t spi_io_transfer_dma(spi_io_t *me, const uint8_t *pui8TxBuff, uint8_t *pui8RxBuff, uint32_t ui32Length);
uint32_t spi_io_write_and_read_dma(spi_io_t *me,
                                   const uint8_t *pui8TxBuff, uint32_t ui32TxLength,
                                   uint8_t *pui8RxBuff, uint32_t ui32RxLength);