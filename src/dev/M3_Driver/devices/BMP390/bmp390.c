/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/* Board Support includes. */
#include "bmp390.h"
#include "define.h"

/* Component includes. */
// #include "i2c_io.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// #define BMP390_ADDR                     0xEE
//#define BMP390_ADDR                     0x77
#define BMP390_STATUS_REG_ADDR			0x03

#define BMP390_PRESSURE_REG_ADDR_BASE	0x04
#define BMP390_PRESSURE_REG_SIZE		3

#define BMP390_TEMP_REG_ADDR_BASE		0x07
#define BMP390_TEMP_REG_SIZE			3

#define BMP390_ALTITUDE_REG_ADDR_BASE	0x07
#define BMP390_ALTITUDE_REG_SIZE		3

#define BMP390_READ_ALL_ADDR_BASE		0x04
#define BMP390_READ_ALL_REG_SIZE		6

#define SEALEVEL_PRESSURE_HPA 			1007

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef enum _Sensor_Read_typedef_
{
	/* :::::::::: BMP390 Read Data Type :::::::: */
	SENSOR_READ_TEMP,
	SENSOR_READ_PRESSURE,
	SENSOR_READ_ALTITUDE,
	SENSOR_READ_BMP390,

} Sensor_Read_typedef;

typedef enum _Sensor_common_read_state_typedef_
{
	SENSOR_READ_RESET_STATE,
	SENSOR_CHECK_STATUS_STATE,
	SENSOR_READ_DATA_STATE,
	SENSOR_PROCESS_DATA_STATE,

} Sensor_common_read_state_typedef;

typedef struct _BMP390_uncomp_data_typedef_
{
	uint16_t 	NVM_PAR_T1;
    uint16_t 	NVM_PAR_T2;
    int8_t 		NVM_PAR_T3;

    int16_t 	NVM_PAR_P1;
    int16_t 	NVM_PAR_P2;
    int8_t 		NVM_PAR_P3;
    int8_t 		NVM_PAR_P4;
    uint16_t 	NVM_PAR_P5;
    uint16_t 	NVM_PAR_P6;
    int8_t 		NVM_PAR_P7;
    int8_t 		NVM_PAR_P8;
    int16_t 	NVM_PAR_P9;
    int8_t 		NVM_PAR_P10;
    int8_t 		NVM_PAR_P11;

} BMP390_uncomp_data_typedef;

typedef struct _BMP390_comp_data_typedef_
{
	float PAR_T1;
	float PAR_T2;
	float PAR_T3;

	float PAR_P1;
	float PAR_P2;
	float PAR_P3;
	float PAR_P4;
	float PAR_P5;
	float PAR_P6;
	float PAR_P7;
	float PAR_P8;
	float PAR_P9;
	float PAR_P10;
	float PAR_P11;

} BMP390_comp_data_typedef;

typedef struct _BMP390_data_typedef_
{
	BMP390_uncomp_data_typedef uncomp_data;
	BMP390_comp_data_typedef   comp_data;

} BMP390_data_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static BMP390_comp_data_typedef BMP390_comp_data;

static bool is_BMP390_Init_Complete  = false;
// static uint8_t is_BMP390_Write_Complete = 0;
// static uint8_t is_BMP390_Read_Complete  = 0;
static bool is_BMP390_Calib_Complete = false;

static bool is_BMP390_Data_Complete  = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static bool BMP390_is_value_ready
(
    Sensor_Read_typedef read_type,
	uint8_t *p_status_value
);

static void BMP390_read_uncompensated_value(bmp390_dev_t* dev, Sensor_Read_typedef read_type, uint8_t *p_BMP390_RX_buffer);

static void BMP390_compensate_value(Sensor_Read_typedef read_type, uint8_t *p_BMP390_RX_buffer);

static void BMP390_compensate_temp(float uncomp_temp);
static void BMP390_compensate_pressure(float uncomp_pressure);
// static void BMP390_compensate_altitude(void);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
float Sensor_Temp = 0.0;
float Sensor_Pressure = 0.0;
// float Sensor_Altitude = 0.0;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
uint8_t BMP390_Write(bmp390_dev_t* dev, uint8_t reg, uint8_t TX_data)
{
	uint8_t frame[2] = {reg, TX_data};

	uint32_t n = i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char*)frame, 2);

	return (n == 2) ? 1u : 0u;
}

uint8_t BMP390_Read(bmp390_dev_t* dev, uint8_t reg, uint8_t *p_RX_buffer, uint8_t byte_count)
{
	// 1) Write the register pointer WITHOUT STOP
    if (i2c_io_send(dev->i2c_bus, dev->slaveAdd, (const char*)&reg, 1) != 0u)
	{
		return ERROR_FAIL;
	}

    // 2) Next recv performs a REPEATED-START automatically, then STOP
    uint32_t n = i2c_io_recv(dev->i2c_bus, dev->slaveAdd, (char*)p_RX_buffer, (int)byte_count);

    return (n == (uint32_t)byte_count) ? 1u : 0u;
}

/* :::::::::: BMP390 Command :::::::: */
uint8_t BMP390_init(bmp390_dev_t* dev)
{
	BMP390_uncomp_data_typedef BMP390_uncomp_data;

	uint8_t Sensor_temp_buffer[30] = {0};

	memset((void*)&BMP390_uncomp_data, 0, sizeof(BMP390_uncomp_data));
	memset((void*)&BMP390_comp_data, 0, sizeof(BMP390_comp_data));

	BMP390_Read(dev, 0x00, Sensor_temp_buffer, 1);

	//set oversampling
	//Sensor_temp_buffer[0] = 11;
	Sensor_temp_buffer[0] = 0;
	Sensor_temp_buffer[0] |= (0b101 << 2); 	// osr_p [2..0] = 101
	Sensor_temp_buffer[0] |= (0b010 << 5); 	// osr4_t[5..3] = 010

	BMP390_Write(dev, 0x1C, Sensor_temp_buffer[0]);

	//set filter
	BMP390_Read(dev, 0x1F, Sensor_temp_buffer, 1);

	//Sensor_temp_buffer[0] = (0b111 << 3); // iir_filter[3..1] = 111
	Sensor_temp_buffer[0] = (0b011 << 3); // iir_filter[3..1] = 011
	//Sensor_temp_buffer[0] = (0b000 << 3); // iir_filter[3..1] = 000
	BMP390_Write(dev, 0x1F, Sensor_temp_buffer[0]);

	BMP390_Read(dev, 0x31, Sensor_temp_buffer, 21);

	BMP390_uncomp_data.NVM_PAR_T1 = (Sensor_temp_buffer[1] << 8)
			| Sensor_temp_buffer[0];
	BMP390_uncomp_data.NVM_PAR_T2 = (Sensor_temp_buffer[3] << 8)
			| Sensor_temp_buffer[2];
	BMP390_uncomp_data.NVM_PAR_T3 = Sensor_temp_buffer[4];
	BMP390_uncomp_data.NVM_PAR_P1 = (Sensor_temp_buffer[6] << 8)
			| Sensor_temp_buffer[5];
	BMP390_uncomp_data.NVM_PAR_P2 = (Sensor_temp_buffer[8] << 8)
			| Sensor_temp_buffer[7];
	BMP390_uncomp_data.NVM_PAR_P3 = Sensor_temp_buffer[9];
	BMP390_uncomp_data.NVM_PAR_P4 = Sensor_temp_buffer[10];
	BMP390_uncomp_data.NVM_PAR_P5 = (Sensor_temp_buffer[12] << 8)
			| Sensor_temp_buffer[11];
	BMP390_uncomp_data.NVM_PAR_P6 = (Sensor_temp_buffer[14] << 8)
			| Sensor_temp_buffer[13];
	BMP390_uncomp_data.NVM_PAR_P7 = Sensor_temp_buffer[15];
	BMP390_uncomp_data.NVM_PAR_P8 = Sensor_temp_buffer[16];
	BMP390_uncomp_data.NVM_PAR_P9 = (Sensor_temp_buffer[18] << 8)
			| Sensor_temp_buffer[17];
	BMP390_uncomp_data.NVM_PAR_P10 = Sensor_temp_buffer[19];
	BMP390_uncomp_data.NVM_PAR_P11 = Sensor_temp_buffer[20];

	float comp_value = 0.00390625f;
	BMP390_comp_data.PAR_T1 =
			((float) (BMP390_uncomp_data.NVM_PAR_T1) / comp_value);

	BMP390_comp_data.PAR_T1 =
			((float) (BMP390_uncomp_data.NVM_PAR_T1) / comp_value);

	comp_value = 1073741824.0f;
	BMP390_comp_data.PAR_T2 =
			((float) (BMP390_uncomp_data.NVM_PAR_T2) / comp_value);

	comp_value = 281474976710656.0f;
	BMP390_comp_data.PAR_T3 =
			((float) (BMP390_uncomp_data.NVM_PAR_T3) / comp_value);

	comp_value = 1048576.0f;
	BMP390_comp_data.PAR_P1 =
			((float) (BMP390_uncomp_data.NVM_PAR_P1 - 16384)
					/ comp_value);

	comp_value = 536870912.0f;
	BMP390_comp_data.PAR_P2 =
			((float) (BMP390_uncomp_data.NVM_PAR_P2 - 16384)
					/ comp_value);

	comp_value = 4294967296.0f;
	BMP390_comp_data.PAR_P3 =
			((float) (BMP390_uncomp_data.NVM_PAR_P3) / comp_value);

	comp_value = 137438953472.0f;
	BMP390_comp_data.PAR_P4 =
			((float) (BMP390_uncomp_data.NVM_PAR_P4) / comp_value);

	comp_value = 0.125f;
	BMP390_comp_data.PAR_P5 =
			((float) (BMP390_uncomp_data.NVM_PAR_P5) / comp_value);

	comp_value = 64.0f;
	BMP390_comp_data.PAR_P6 =
			((float) (BMP390_uncomp_data.NVM_PAR_P6) / comp_value);

	comp_value = 256.0f;
	BMP390_comp_data.PAR_P7 =
			((float) (BMP390_uncomp_data.NVM_PAR_P7) / comp_value);

	comp_value = 32768.0f;
	BMP390_comp_data.PAR_P8 =
			((float) (BMP390_uncomp_data.NVM_PAR_P8) / comp_value);

	comp_value = 281474976710656.0f;
	BMP390_comp_data.PAR_P9 =
			((float) (BMP390_uncomp_data.NVM_PAR_P9) / comp_value);

	BMP390_comp_data.PAR_P10 =
			((float) (BMP390_uncomp_data.NVM_PAR_P10) / comp_value);

	comp_value = 36893488147419103232.0f;
	BMP390_comp_data.PAR_P11 =
			((float) (BMP390_uncomp_data.NVM_PAR_P11) / comp_value);

	
	memset((void*)Sensor_temp_buffer, 0, sizeof(Sensor_temp_buffer));

	BMP390_Read(dev, 0x1B, Sensor_temp_buffer, 1);

	Sensor_temp_buffer[0] |= 0b00010011;
	BMP390_Write(dev, 0x1B, Sensor_temp_buffer[0]);

	// BMP390_read_uncompensated_value(p_i2c, SENSOR_READ_BMP390, &Sensor_temp_buffer[1]);

	// BMP390_compensate_value(SENSOR_READ_BMP390, &Sensor_temp_buffer[1]);

	//is_sensor_read_enable = false;

	is_BMP390_Init_Complete = true;
	return ERROR_OK;
}

uint8_t BMP390_read_value(bmp390_dev_t* dev, bmp390_data_t* p_data)
{
	uint8_t Sensor_temp_buffer[30] = {0};

	BMP390_Read(dev, 0x1B, Sensor_temp_buffer, 1);

	Sensor_temp_buffer[0] |= 0b00010011;
	BMP390_Write(dev, 0x1B, Sensor_temp_buffer[0]);

	BMP390_Read(dev, BMP390_STATUS_REG_ADDR, Sensor_temp_buffer, 1);

	if (BMP390_is_value_ready(SENSOR_READ_BMP390, Sensor_temp_buffer) == false)
	{
		return ERROR_FAIL;
	}

	BMP390_read_uncompensated_value(dev, SENSOR_READ_BMP390, &Sensor_temp_buffer[1]);

	BMP390_compensate_value(SENSOR_READ_BMP390, &Sensor_temp_buffer[1]);

	p_data->Temp 	 = Sensor_Temp;
	p_data->Pressure = Sensor_Pressure;

	//is_sensor_read_enable = false;

    dev->init_status = true;
	return ERROR_OK;
}

/* :::::::::: BMP390 Flag Check Command :::::::: */
uint32_t Is_BMP390_Init_Complete(bmp390_dev_t* dev)
{
    if (dev->init_status == true)
    {
        return ERROR_OK;
    }
    return ERROR_FAIL;
}

bool Is_BMP390_Calib_Complete()
{
    if (is_BMP390_Calib_Complete == true)
    {
        is_BMP390_Calib_Complete = false;
        return ERROR_OK;
    }
    
    return ERROR_FAIL;
}

/* :::::::::: Sensor_BMP390 Interface :::::::: */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static bool BMP390_is_value_ready(Sensor_Read_typedef read_type, uint8_t *p_status_value)
{
	switch (read_type)
    {
	case SENSOR_READ_PRESSURE:
	case SENSOR_READ_ALTITUDE:
		return (*p_status_value & (1 << 4)) && (*p_status_value & (1 << 5));

	case SENSOR_READ_TEMP:
		return (*p_status_value & (1 << 4)) && (*p_status_value & (1 << 6));

	case SENSOR_READ_BMP390:
		return (*p_status_value & (1 << 4)) && (*p_status_value & (1 << 5))
				&& (*p_status_value & (1 << 6));

	default:
		break;
	}
	return ERROR_FAIL;
}

static void BMP390_read_uncompensated_value(bmp390_dev_t* dev, Sensor_Read_typedef read_type, uint8_t *p_BMP390_RX_buffer)
{
	if (read_type == SENSOR_READ_TEMP)
    {
		BMP390_Read(dev, BMP390_TEMP_REG_ADDR_BASE, p_BMP390_RX_buffer, BMP390_TEMP_REG_SIZE);
		return;
	}

	BMP390_Read(dev, BMP390_READ_ALL_ADDR_BASE, p_BMP390_RX_buffer, BMP390_READ_ALL_REG_SIZE);
}

static void BMP390_compensate_value(Sensor_Read_typedef read_type, uint8_t *p_BMP390_RX_buffer)
{
	switch (read_type)
    {
	case SENSOR_READ_TEMP:
		BMP390_compensate_temp(
				(p_BMP390_RX_buffer[2] << 16) | (p_BMP390_RX_buffer[1] << 8)
						| p_BMP390_RX_buffer[0]);
		break;

	case SENSOR_READ_PRESSURE:
		BMP390_compensate_temp(
					(p_BMP390_RX_buffer[5] << 16) | (p_BMP390_RX_buffer[4] << 8)
							| p_BMP390_RX_buffer[3]);
		BMP390_compensate_pressure(
				(p_BMP390_RX_buffer[2] << 16) | (p_BMP390_RX_buffer[1] << 8)
						| p_BMP390_RX_buffer[0]);
		break;

	case SENSOR_READ_ALTITUDE:
	case SENSOR_READ_BMP390:
		BMP390_compensate_temp(
				(p_BMP390_RX_buffer[5] << 16) | (p_BMP390_RX_buffer[4] << 8)
						| p_BMP390_RX_buffer[3]);

		BMP390_compensate_pressure(
				(p_BMP390_RX_buffer[2] << 16) | (p_BMP390_RX_buffer[1] << 8)
						| p_BMP390_RX_buffer[0]);
		// BMP390_compensate_altitude();
		break;

	default:
		break;
	}
}

static void BMP390_compensate_temp(float uncomp_temp)
{
	float PAR_T1;
	float PAR_T2;

	PAR_T1 = (uncomp_temp - BMP390_comp_data.PAR_T1);
	PAR_T2 = (PAR_T1 * BMP390_comp_data.PAR_T2);

	Sensor_Temp = PAR_T2 + (PAR_T1 * PAR_T1) * BMP390_comp_data.PAR_T3;
}

static void BMP390_compensate_pressure(float uncomp_pressure)
{
	float PAR_T1;
	float PAR_T2;
	float PAR_T3;

	float PAR_OUT1;
	float PAR_OUT2;
	float PAR_OUT3;

	PAR_T1 = BMP390_comp_data.PAR_P6 * (Sensor_Temp);
	PAR_T2 = BMP390_comp_data.PAR_P7 * (Sensor_Temp * Sensor_Temp);
	PAR_T3 = BMP390_comp_data.PAR_P8 * (Sensor_Temp * Sensor_Temp * Sensor_Temp);

	PAR_OUT1 = BMP390_comp_data.PAR_P5 + PAR_T1 + PAR_T2 + PAR_T3;

	PAR_T1 = BMP390_comp_data.PAR_P2 * (Sensor_Temp);
	PAR_T2 = BMP390_comp_data.PAR_P3 * (Sensor_Temp * Sensor_Temp);
	PAR_T3 = BMP390_comp_data.PAR_P4 * (Sensor_Temp * Sensor_Temp * Sensor_Temp);
	
	PAR_OUT2 = uncomp_pressure * (BMP390_comp_data.PAR_P1 + PAR_T1 + PAR_T2 + PAR_T3);

	PAR_T1 = uncomp_pressure * uncomp_pressure;
	PAR_T2 = BMP390_comp_data.PAR_P9 + BMP390_comp_data.PAR_P10 * Sensor_Temp;
	PAR_T3 = PAR_T1 * PAR_T2;
	PAR_OUT3 = PAR_T3 + (uncomp_pressure * uncomp_pressure * uncomp_pressure) * BMP390_comp_data.PAR_P11;

	Sensor_Pressure = PAR_OUT1 + PAR_OUT2 + PAR_OUT3;
}

// static void BMP390_compensate_altitude(void)
// {
// 	float atmospheric = Sensor_Pressure / 100.0F;
//  	Sensor_Altitude =  44330.0 * (1.0 - pow(atmospheric / SEALEVEL_PRESSURE_HPA, 0.1903));
// }

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */