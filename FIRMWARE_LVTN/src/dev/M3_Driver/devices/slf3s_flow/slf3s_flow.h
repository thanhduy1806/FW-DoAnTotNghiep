/*
 * slf3s_flow.h
 *
 * Driver for Sensirion SLF3S-0600F Liquid Flow Sensor
 * Created on: Oct 21, 2025
 * Author: Grok
 */

#ifndef SLF3S_FLOW_H_
#define SLF3S_FLOW_H_

#include "i2c_io.h"  // Assume from previous module
#include "stdint.h"
#include "stdbool.h"

// I2C 7-bit address
#define SLF3S_I2C_ADDR             (0x08)

// Commands (16-bit)
#define SLF3S_CMD_START_H2O        (0x3608)
#define SLF3S_CMD_START_IPA        (0x3615)
#define SLF3S_CMD_STOP             (0x3FF9)
#define SLF3S_CMD_READ_ID_1        (0x367C)
#define SLF3S_CMD_READ_ID_2        (0xE102)
#define SLF3S_CMD_SOFT_RESET       (0x0006)  // 8-bit, general call addr 0x00

// Scale factors
#define SLF3S_FLOW_SCALE           (10.0f)   // ul/min
#define SLF3S_TEMP_SCALE           (200.0f)  // °C

// Error codes
#define SLF3S_OK                   ( 0)
#define SLF3S_ERROR                (-1)
#define SLF3S_CRC_ERROR            (-2)
#define SLF3S_I2C_ERROR            (-3)

// Product ID for SLF3S-0600F
#define SLF3S_PRODUCT_ID           (0x07030302UL)

// Struct for all sensor readings
typedef struct {
    float flow;      // Flow rate in µl/min
    float temp;      // Temperature in °C
    uint16_t flags;  // Status flags (bit 0: Air-in-Line, bit 1: High Flow, bit 5: Smoothing)
} slf3s_readings_t;

// API declarations
int32_t slf3s_init(i2c_io_t *i2c, bool is_water);  // is_water=true for H2O, false for IPA
int32_t slf3s_stop(i2c_io_t *i2c);
int32_t slf3s_soft_reset(i2c_io_t *i2c);
float slf3s_read_flow(i2c_io_t *i2c);
float slf3s_read_temperature(i2c_io_t *i2c);
uint16_t slf3s_read_flags(i2c_io_t *i2c);
int32_t slf3s_read_all(i2c_io_t *i2c, slf3s_readings_t *readings);  // New: Read all 3 values
uint32_t slf3s_get_product_id(i2c_io_t *i2c);
uint64_t slf3s_get_serial_number(i2c_io_t *i2c);
bool slf3s_validate_crc(const uint8_t *data, uint8_t crc_received);  // CRC for 2 data bytes

#endif /* SLF3S_FLOW_H_ */
