/* 
 * File:   mb85rs2mt.h
 * Author: HTSANG
 *
 * Created on March 25, 2026, 9:50 AM
 */

#ifndef MB85RS2MT_H
#define	MB85RS2MT_H

#include <stdint.h>
#include "spi_io.h"
#include "do.h"
#include "os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "definitions.h"                // SYS function prototypes


/* -------- Commands -------- */

#define MB85RS2MT_CMD_WREN     0x06  /* Enable Write */
#define MB85RS2MT_CMD_WRDI     0x04  /* Disable Write */
#define MB85RS2MT_CMD_RDSR     0x05  /* Read Status */
#define MB85RS2MT_CMD_WRSR     0x01  /* Write Status */
#define MB85RS2MT_CMD_READ     0x03  /* Read Data */
#define MB85RS2MT_CMD_WRITE    0x02  /* Write Data */
#define MB85RS2MT_CMD_SLEEP    0xB9  /* Sleep Mode */
#define MB85RS2MT_CMD_RDID     0xAB  /* Wake Up */

/* -------- Device Struct -------- */

typedef struct mb85rs2mt_dev
{
    spi_io_t *spi;   /* SPI abstraction */
    do_t     *cs;    /* Chip select */
} mb85rs2mt_dev_t;

int mb85rs2mt_write(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);

int mb85rs2mt_read(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);

#endif	/* MB85RS2MT_H */

