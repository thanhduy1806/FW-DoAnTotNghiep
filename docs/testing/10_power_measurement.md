# Power Measurement Module (PWR MEAS)

## Expected coverage

- Read current from power channels
- Read voltage from TEC channels
- Read raw ADC values for debugging
- Monitor switch status

---

## Core commands

| Command | Arguments | Expected Response |
|---|---|---|
| get_i_pwr | number | current value (A)|
| get_v_tec | number | voltage value (V)|
| get_adc_raw | number | raw ADC value |
| sw_status | none | status |

---

## Recommended command behavior

### `get_i_pwr number`
Read current value of selected power channel.

- number: channel index

| Number | Power |
|---|---|
|1  | 12V_IF |
|2  | 12V_HEATER |
|3  | 12V_LP |
|4  | 5V_SOM |
|5  | 5V_TEC |
|6  | 5V_HD4 |
|7  | 3/5V_SOLENOID |


Example:  Read current value of 12V_IF
> get_i_pwr  1

---

### `get_v_tec <number>`
Read voltage value of TEC channel.

- number: channel index

| Number | TEC |
|---|---|
|9  | TEC_ADC_1 |
|10  | TEC_ADC_2 |
|11  | TEC_ADC_3 |
|12  | TEC_ADC_4 |

Example: Read voltage value of TEC_ADC_2

> get_v_tec 10

---

### `get_adc_raw <number>`
Read raw ADC value (for debug / calibration).

- number: channel index

| Number | ADC |
|---|---|
|1  | 12V_IF |
|2  | 12V_HEATER |
|3  | 12V_LP |
|4  | 5V_SOM |
|5  | 5V_TEC |
|6  | 5V_HD4 |
|7  | 3/5V_SOLENOID |
|8  | NO CONNECT |
|9  | TEC_ADC_1 |
|10  | TEC_ADC_2 |
|11  | TEC_ADC_3 |
|12  | TEC_ADC_4 |

Example:
> get_adc_raw  2

---


### `sw_status`

Get status of all ADC switches.

