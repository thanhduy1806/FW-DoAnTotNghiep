/*
 * samv71_i2c_io.c
 *
 * SAMV71 I2C (TWIHS) synchronous IO
 * - No locking in IO layer (application manages concurrency)
 *
 * Dependencies:
 *  - i2c_io.h
 *  - define.h (Std_ReturnType)
 *  - device header that defines:
 *      twihs_registers_t, pmc_registers_t
 *      TWIHS0_REGS/TWIHS1_REGS/TWIHS2_REGS
 *      PMC_REGS
 *      ID_TWIHS0/ID_TWIHS1/ID_TWIHS2
 *      TWIHS_* bit macros (from component/twihs.h)
 *  - os_hal.h provides:
 *      os_reg_wait_flag_blocking(const volatile uint32_t *reg, uint32_t mask, uint32_t timeout_ms)
 */

#include "i2c_io.h"
#include "define.h"
#include "os_hal.h"
#include <stdint.h>
#include <stdbool.h>

// Include your device header (the one you showed)
#include "samv71q21b.h"

// ---------------- Config ----------------
#ifndef I2C_IO_DEFAULT_SPEED_HZ
#define I2C_IO_DEFAULT_SPEED_HZ   (100000u)
#endif

#ifndef I2C_IO_TIMEOUT_MS
#define I2C_IO_TIMEOUT_MS         (100u)
#endif

#ifndef I2C_IO_MAX_PORTS
#define I2C_IO_MAX_PORTS          (3u)
#endif

// ---------------- Port mapping ----------------
// Assumption: ui32I2cPort: 0->TWIHS0, 1->TWIHS1, 2->TWIHS2
static twihs_registers_t * const s_twihs[I2C_IO_MAX_PORTS] =
{
    TWIHS0_REGS,
    TWIHS1_REGS,
    TWIHS2_REGS
};

static inline uint32_t twihs_id_from_port(uint32_t port)
{
    switch (port)
    {
        case 0: return ID_TWIHS0;
        case 1: return ID_TWIHS1;
        case 2: return ID_TWIHS2;
        default: return 0xFFFFFFFFu;
    }
}

static inline Std_ReturnType wait_set(const volatile uint32_t *reg, uint32_t mask, uint32_t timeout_ms)
{
    return (Std_ReturnType)os_reg_wait_flag(reg, mask, timeout_ms);
}

static inline bool twihs_has_nack(twihs_registers_t *twi)
{
    return ((twi->TWIHS_SR & TWIHS_SR_NACK_Msk) != 0u);
}

static inline void twihs_enable_clock(uint32_t port)
{
    uint32_t id = twihs_id_from_port(port);
    if (id == 0xFFFFFFFFu) return;

    // Enable peripheral clock in PMC
    // IDs < 32 -> PCER0, else PCER1
    if (id < 32u)
        PMC_REGS->PMC_PCER0 = (1u << id);
    else
        PMC_REGS->PMC_PCER1 = (1u << (id - 32u));
}

void TWIHS2_Initialize( void )
{
    // Reset the i2c Module
    TWIHS2_REGS->TWIHS_CR = TWIHS_CR_SWRST_Msk;

    // Disable the I2C Master/Slave Mode
    TWIHS2_REGS->TWIHS_CR = TWIHS_CR_MSDIS_Msk | TWIHS_CR_SVDIS_Msk;

    // Set Baud rate
    TWIHS2_REGS->TWIHS_CWGR = (TWIHS_CWGR_HOLD_Msk & TWIHS2_REGS->TWIHS_CWGR) | TWIHS_CWGR_CLDIV(153) | TWIHS_CWGR_CHDIV(141) | TWIHS_CWGR_CKDIV(0);

    // Starts the transfer by clearing the transmit hold register
    TWIHS2_REGS->TWIHS_CR = TWIHS_CR_THRCLR_Msk;

    // Disable TXRDY, TXCOMP and RXRDY interrupts
    TWIHS2_REGS->TWIHS_IDR = TWIHS_IDR_TXCOMP_Msk | TWIHS_IDR_TXRDY_Msk | TWIHS_IDR_RXRDY_Msk;

    // Enables interrupt on nack and arbitration lost
    TWIHS2_REGS->TWIHS_IER = TWIHS_IER_NACK_Msk | TWIHS_IER_ARBLST_Msk;

    // Enable Master Mode
    TWIHS2_REGS->TWIHS_CR = TWIHS_CR_MSEN_Msk;
}

void TWIHS1_Initialize( void )
{
    // Reset the i2c Module
    TWIHS1_REGS->TWIHS_CR = TWIHS_CR_SWRST_Msk;

    // Disable the I2C Master/Slave Mode
    TWIHS1_REGS->TWIHS_CR = TWIHS_CR_MSDIS_Msk | TWIHS_CR_SVDIS_Msk;

    // Set Baud rate
    TWIHS1_REGS->TWIHS_CWGR = (TWIHS_CWGR_HOLD_Msk & TWIHS2_REGS->TWIHS_CWGR) | TWIHS_CWGR_CLDIV(153) | TWIHS_CWGR_CHDIV(141) | TWIHS_CWGR_CKDIV(0);

    // Starts the transfer by clearing the transmit hold register
    TWIHS1_REGS->TWIHS_CR = TWIHS_CR_THRCLR_Msk;

    // Disable TXRDY, TXCOMP and RXRDY interrupts
    TWIHS1_REGS->TWIHS_IDR = TWIHS_IDR_TXCOMP_Msk | TWIHS_IDR_TXRDY_Msk | TWIHS_IDR_RXRDY_Msk;

    // Enables interrupt on nack and arbitration lost
    TWIHS1_REGS->TWIHS_IER = TWIHS_IER_NACK_Msk | TWIHS_IER_ARBLST_Msk;

    // Enable Master Mode
    TWIHS1_REGS->TWIHS_CR = TWIHS_CR_MSEN_Msk;
}

void TWIHS0_Initialize( void )
{
    // Reset the i2c Module
    TWIHS0_REGS->TWIHS_CR = TWIHS_CR_SWRST_Msk;

    // Disable the I2C Master/Slave Mode
    TWIHS0_REGS->TWIHS_CR = TWIHS_CR_MSDIS_Msk | TWIHS_CR_SVDIS_Msk;

    // Set Baud rate
    TWIHS0_REGS->TWIHS_CWGR = (TWIHS_CWGR_HOLD_Msk & TWIHS2_REGS->TWIHS_CWGR) | TWIHS_CWGR_CLDIV(153) | TWIHS_CWGR_CHDIV(141) | TWIHS_CWGR_CKDIV(0);

    // Starts the transfer by clearing the transmit hold register
    TWIHS0_REGS->TWIHS_CR = TWIHS_CR_THRCLR_Msk;

    // Disable TXRDY, TXCOMP and RXRDY interrupts
    TWIHS0_REGS->TWIHS_IDR = TWIHS_IDR_TXCOMP_Msk | TWIHS_IDR_TXRDY_Msk | TWIHS_IDR_RXRDY_Msk;

    // Enables interrupt on nack and arbitration lost
    TWIHS0_REGS->TWIHS_IER = TWIHS_IER_NACK_Msk | TWIHS_IER_ARBLST_Msk;

    // Enable Master Mode
    TWIHS0_REGS->TWIHS_CR = TWIHS_CR_MSEN_Msk;
}

/*
 * Program TWIHS CWGR for approximate speed.
 * Uses CLDIV=CHDIV and increases CKDIV until CLDIV fits in 8-bit.
 *
 * NOTE:
 *  - Needs CPU/MCK clock (Hz). If you already have a function that returns MCK,
 *    plug it here. If not, define I2C_IO_MCK_HZ from your system config.
 */
#ifndef I2C_IO_MCK_HZ
// Fallback: you SHOULD override this to the real MCK in your project
#define I2C_IO_MCK_HZ (150000000u)   // example only (set correctly!)
#endif

static void twihs_set_speed(twihs_registers_t *twi, uint32_t mck_hz, uint32_t scl_hz)
{
    if (scl_hz == 0u) scl_hz = I2C_IO_DEFAULT_SPEED_HZ;
    if (mck_hz == 0u) mck_hz = I2C_IO_MCK_HZ;

    uint32_t ckdiv = 0u;
    uint32_t div;

    while (1)
    {
        // base ~ mck/(2*scl) - 3
        uint32_t base = mck_hz / (2u * scl_hz);
        base = (base > 3u) ? (base - 3u) : 0u;

        div = base >> ckdiv;

        if (div <= 255u) break;
        ckdiv++;
        if (ckdiv >= 7u) { div = 255u; break; }
    }

    twi->TWIHS_CWGR =
        TWIHS_CWGR_CLDIV(div) |
        TWIHS_CWGR_CHDIV(div) |
        TWIHS_CWGR_CKDIV(ckdiv);
}

static Std_ReturnType wait_txcomp_or_nack(twihs_registers_t *twi, uint32_t timeout_ms)
{
    if (twihs_has_nack(twi)) return ERROR_I2C_NACK;

    Std_ReturnType rc = wait_set(&twi->TWIHS_SR, TWIHS_SR_TXCOMP_Msk, timeout_ms);
    if (rc != ERROR_OK) return rc;

    if (twihs_has_nack(twi)) return ERROR_I2C_NACK;
    return ERROR_OK;
}

// ---------------- Public APIs ----------------

uint32_t i2c_io_enable(i2c_io_t *me)
{
    if (me == NULL) return ERROR_INVALID_PARAM;
    if (me->ui32I2cPort >= I2C_IO_MAX_PORTS) return ERROR_INVALID_PARAM;

    twihs_registers_t *twi = s_twihs[me->ui32I2cPort];
    if (twi == NULL) return ERROR_INVALID_PARAM;

    twihs_enable_clock(me->ui32I2cPort);

    // Software reset
    twi->TWIHS_CR = TWIHS_CR_SWRST_Msk;
    (void)twi->TWIHS_RHR; // dummy read

    // Enable master, disable slave
    twi->TWIHS_CR = TWIHS_CR_MSEN_Msk | TWIHS_CR_SVDIS_Msk;

    // Default speed
    twihs_set_speed(twi, I2C_IO_MCK_HZ, I2C_IO_DEFAULT_SPEED_HZ);

    return ERROR_OK;
}

uint32_t i2c_io_send(i2c_io_t *me, uint8_t ui8SlaveAddr, const char *buf, int count)
{
    if (me == NULL || buf == NULL || count <= 0) return ERROR_INVALID_PARAM;
    if (me->ui32I2cPort >= I2C_IO_MAX_PORTS) return ERROR_INVALID_PARAM;

    twihs_registers_t *twi = s_twihs[me->ui32I2cPort];
    if (twi == NULL) return ERROR_INVALID_PARAM;

    // Configure for write:
    // DADR (7-bit), MREAD=0, IADRSZ=0
    twi->TWIHS_MMR = TWIHS_MMR_DADR(ui8SlaveAddr) | TWIHS_MMR_IADRSZ(0u);
    twi->TWIHS_IADR = 0u;

    for (int i = 0; i < count; i++)
    {
        Std_ReturnType rc = wait_set(&twi->TWIHS_SR, TWIHS_SR_TXRDY_Msk, I2C_IO_TIMEOUT_MS);
        if (rc != ERROR_OK) return rc;

        if (twihs_has_nack(twi))
        {
            // Release bus
            twi->TWIHS_CR = TWIHS_CR_STOP_Msk;
            (void)wait_txcomp_or_nack(twi, I2C_IO_TIMEOUT_MS);
            return ERROR_I2C_NACK;
        }

        twi->TWIHS_THR = (uint8_t)buf[i];
    }

    twi->TWIHS_CR = TWIHS_CR_STOP_Msk;
    return wait_txcomp_or_nack(twi, I2C_IO_TIMEOUT_MS);
}

uint32_t i2c_io_recv(i2c_io_t *me, uint8_t ui8SlaveAddr, char *buf, int count)
{
    if (me == NULL || buf == NULL || count <= 0) return ERROR_INVALID_PARAM;
    if (me->ui32I2cPort >= I2C_IO_MAX_PORTS) return ERROR_INVALID_PARAM;

    twihs_registers_t *twi = s_twihs[me->ui32I2cPort];
    if (twi == NULL) return ERROR_INVALID_PARAM;

    // Configure for read:
    // DADR (7-bit), MREAD=1, IADRSZ=0
    twi->TWIHS_MMR = TWIHS_MMR_DADR(ui8SlaveAddr) | TWIHS_MMR_MREAD_Msk | TWIHS_MMR_IADRSZ(0u);
    twi->TWIHS_IADR = 0u;

    if (count == 1)
    {
        // START + STOP for single byte
        twi->TWIHS_CR = TWIHS_CR_START_Msk | TWIHS_CR_STOP_Msk;

        Std_ReturnType rc = wait_set(&twi->TWIHS_SR, TWIHS_SR_RXRDY_Msk, I2C_IO_TIMEOUT_MS);
        if (rc != ERROR_OK) return rc;

        if (twihs_has_nack(twi)) return ERROR_I2C_NACK;

        buf[0] = (char)(twi->TWIHS_RHR & 0xFFu);

        return wait_txcomp_or_nack(twi, I2C_IO_TIMEOUT_MS);
    }

    // Multi-byte read
    twi->TWIHS_CR = TWIHS_CR_START_Msk;

    for (int i = 0; i < count; i++)
    {
        if (i == (count - 1))
        {
            // STOP before last byte is received (SAM convention)
            twi->TWIHS_CR = TWIHS_CR_STOP_Msk;
        }

        Std_ReturnType rc = wait_set(&twi->TWIHS_SR, TWIHS_SR_RXRDY_Msk, I2C_IO_TIMEOUT_MS);
        if (rc != ERROR_OK) return rc;

        if (twihs_has_nack(twi)) return ERROR_I2C_NACK;

        buf[i] = (char)(twi->TWIHS_RHR & 0xFFu);
    }

    return wait_txcomp_or_nack(twi, I2C_IO_TIMEOUT_MS);
}