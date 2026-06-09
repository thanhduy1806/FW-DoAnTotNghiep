/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* FreeRTOS includes. */
#include "os.h"
/* Board Support includes. */
#include "bsp_core.h"
#include "bsp_pump.h"
//#include "bsp_solenoid.h"

/* Component includes. */
#include "i2c_io.h"
#include "do.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* Bartels mp-Highdriver registers */
#define I2C_HIGHDRIVER_ADRESS (0x78) // Default adress for mp-Highdriver
#define I2C_DEVICEID  0x00
#define I2C_POWERMODE 0x01
#define I2C_FREQUENCY 0x02
#define I2C_SHAPE     0x03
#define I2C_BOOST     0x04
#define I2C_AUDIO     0x05
#define I2C_PVOLTAGE  0x06
#define I2C_P1VOLTAGE 0x06
#define I2C_P2VOLTAGE 0x07
#define I2C_P3VOLTAGE 0x08
#define I2C_P4VOLTAGE 0x09
#define I2C_UPDATEVOLTAGE 0x0A

#ifndef constrain
#define constrain(amt, low, high) (( (amt) < (low) ) ? (low) : ( ((amt) > (high)) ? (high) : (amt) ))
#endif

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern i2c_io_t i2c2;

bool    bPumpState[4];
uint8_t nPumpVoltageByte[4];
uint8_t nFrequencyByte;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void Highdriver_init(void);
static void Highdriver_enable(uint8_t is_enable);
static void Highdriver_setvoltage(uint8_t _voltage); // Set new amplitude (_voltage [Vpp])
static void Highdriver_setfrequency(uint16_t _frequency); // set pump frequency (_frequency [Hz])

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void bsp_pump_init()
{
    do_set(&power_buck_peri);
    
    do_set(&power_hd4);

	do_set(&hd4_en);

    volatile uint32_t delay = 200000; // tuning theo CPU clock
    while (delay--);

    Highdriver_init();

    // Set valve direction to dummy
//    Valve_switch(VALVE_DIRECTION_DUMMY);
}

void bsp_pump_enable(bool is_enable)
{
	Highdriver_enable(is_enable);
}

void bsp_pump_set_voltage(uint8_t volt)
{
	Highdriver_setvoltage(volt);
}

void bsp_pump_set_freq(uint16_t freq)
{
    Highdriver_setfrequency(freq);
}

/* --------- small utils --------- */
// small helper to send [reg | data...] in one I2C burst
static inline void _mp_i2c_write_block(uint8_t startReg, const uint8_t *data, int len)
{
    if (!data || len <= 0)
    {
        return;
    }
    uint8_t buf[1 + 16];

    if (len > 16)
    {
        return;
    }

    buf[0] = startReg;
    for (int i = 0; i < len; ++i)
    {
        buf[1 + i] = data[i];
    }

    (void)i2c_io_send(&i2c2, I2C_HIGHDRIVER_ADRESS, (const char*)buf, 1 + len);
}

/* --------- public API --------- */
static void Highdriver_init(void)
{
    // Build same sequence as Wire.beginTransmission(...) writes:
    // [0x01: POWERMODE=0x01, FREQ=nFrequencyByte, SHAPE=0x00, BOOST=0x00, AUDIO=0x00,
    //  P1=0x00, P2=0x00, P3=0x00, P4=0x00, UPDATE=0x01]
    uint8_t block[] = {
        0x01,               // POWERMODE enable
        0x40,               // FREQUENCY (caller should set; typical 0x40 for 100 Hz)
        0x00,               // SHAPE (sine)
        0x00,               // BOOST (800kHz off)
        0x00,               // AUDIO off
        0x00,               // P1
        0x00,               // P2
        0x00,               // P3
        0x00,               // P4
        0x01                // UPDATE
    };
    _mp_i2c_write_block(I2C_POWERMODE, block, sizeof(block));

    bPumpState[0]        = false;
    nPumpVoltageByte[0]  = 0x1F;
}

static void Highdriver_enable(uint8_t is_enable)
{
    uint8_t v = (is_enable != 0) ? 0x01u : 0x00u;   // normalize to 0/1
    _mp_i2c_write_block(I2C_POWERMODE, &v, 1);      // writes [0x01, v]

    v = 0x01;
    _mp_i2c_write_block(I2C_UPDATEVOLTAGE, &v, 1);
}

static void Highdriver_setvoltage(uint8_t _voltage) // Set new amplitude (_voltage [Vpp])
{
    // 250Vpp -> 0x1F mapping, same float math as reference
    float temp = _voltage;
    temp *= 31.0f;
    temp /= 250.0f;
    nPumpVoltageByte[0] = constrain((int)temp, 0, 31);

    // Original sequence writes starting at 0x06 (P1)?P4? then UPDATE=0x01
    // Here we zero P1..P3 and set P4 (or 0 if pump off), then update.
    uint8_t block[] =
    {
        0x00, // P1
        0x00, // P2
        0x00, // P3
        // (uint8_t)(bPumpState[0] ? nPumpVoltageByte[0] : 0x00), // P4
        nPumpVoltageByte[0], // P4
        0x01  // UPDATE
    };
    _mp_i2c_write_block(I2C_PVOLTAGE, block, sizeof(block));
}

static void Highdriver_setfrequency(uint16_t _frequency) // set pump frequency (_frequency [Hz])
{
    if (_frequency >= 800)
    {
        nFrequencyByte = 0xFF;
    } else if (_frequency >= 400)
    { // 400-800 Hz
        _frequency -= 400;
        _frequency *= 64;
        _frequency /= 400;
        nFrequencyByte = (uint8_t)(_frequency | 0xC0);
    } else if (_frequency >= 200)
    { // 200-400 Hz
        _frequency -= 200;
        _frequency *= 64;
        _frequency /= 200;
        nFrequencyByte = (uint8_t)(_frequency | 0x80);
    } else if (_frequency >= 100)
    { // 100-200 Hz
        _frequency -= 100;
        _frequency *= 64;
        _frequency /= 100;
        nFrequencyByte = (uint8_t)(_frequency | 0x40);
    } else if (_frequency >= 50)
    {  // 50-100 Hz
        _frequency -= 50;
        _frequency *= 64;
        _frequency /= 50;
        nFrequencyByte = (uint8_t)(_frequency | 0x00);
    } else
    { // outside of valid area
        nFrequencyByte = 0x00;
    }

    uint8_t v = nFrequencyByte;
    _mp_i2c_write_block(I2C_FREQUENCY, &v, 1);

    v = 0x01;
    _mp_i2c_write_block(I2C_UPDATEVOLTAGE, &v, 1);
}
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
