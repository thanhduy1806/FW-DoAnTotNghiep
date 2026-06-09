# LSM6DSOX Test

## Expected coverage

- **LSM6DS0X Self Test** — Verify LSM6DSOX IMU is alive by reading WHO_AM_I register. Should return `Check OK` if the device responds with the expected ID (0x6C).

## Core LSM6D-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `lsm6d_conf_accel` | odr_xl fs_xl | Configures Accelerometer ODR and Full Scale |
| `lsm6d_conf_gyro`  | odr_gy fs_gy | Configures Gyroscope ODR and Full Scale |
| `lsm6d_conf_int`   | mode event | Configures Interrupt mode and event |
| `lsm6d_conf_ff`    | ths_ff dur_ff | Sets Free-fall threshold and duration parameters |
| `lsm6d_conf_wu`    | ths_wu dur_wu | Sets Wake-up threshold and duration parameters |
| `lsm6d_init`       | none | Initializes the LSM6DSOX |
| `lsm6d_dis_int`    | none | Disables all active interrupts |
| `lsm6d_check`      | none | Verify LSM6DSOX is alive or not |
| `lsm6d_read_all`   | none | Reads and prints Accel (mg), Gyro (dps), and Temperature (°C) data |
| `lsm6d_read_accel` | none | Reads and outputs 3-axis Accelerometer data (X, Y, Z) in mg |
| `lsm6d_read_gyro`  | none | Reads and outputs 3-axis Gyroscope data (X, Y, Z) in dps |
| `lsm6d_read_temp`  | none | Reads and outputs the internal sensor temperature in Celsius |

## Parameters

| Parameter | Range |     Description     |
|-----------|-------|---------------------|
|  `odr_xl` | 0 – 7 |       ODR Accelerometer         |
|  `fs_xl`  | 0 – 3 |       Full scale Accelerometer  |
|  `odr_gy` | 0 – 7 |       ODR Gyroscope             |
|  `fs_gy`  | 0 – 4 |       Full scale Gyroscope      |
|  `mode`   | 0 – 7 |       Mode Interrupt            |
|  `event`  | 0 – 8 |       Event Interrupt           |
|  `ths_ff` | 0 – 7 |       Threshold for Free-Fall   |
|  `dur_ff` | 0 – 63|       Duration or Free-Fall     |
|  `ths_wu` | 0 – 63|       Threshold for Wake-up     |
|  `dur_wu` | 0 – 3 |       Duration for Wake-up      |


|    ODR    |     Description     |
|-----------|---------------------|
|    `0`    |       Power-down    |
|    `1`    |       12.5 Hz       |
|    `2`    |       26 Hz         |
|    `3`    |       52 Hz         |
|    `4`    |       104 Hz        |
|    `5`    |       208 Hz        |
|    `6`    |       416 Hz        |
|    `7`    |       833 Hz        |

|   FS GY   |     Full Scale Gyroscope     |   FS XL   |     Full Scale Accelerometer     |
|-----------|---------------------|-----------|---------------------|
|    `0`    |       125 dps       |    `0`    |       2 g           |
|    `1`    |       250 dps       |    `1`    |       16 g          |
|    `2`    |       500 dps       |    `2`    |       4 g           |
|    `3`    |       1000 dp       |    `3`    |       8 g           |
|    `4`    |       2000 dps      |           |                     |

|   THS FF  |     Threshold free-fall     |   THS WU  |     Threshold wake-up     |
|-----------|---------------------|-----------|---------------------|
|    `0`    |        156 mg       |    `0`    |  0 x Full Scale Accelerometer/2^6   |
|    `1`    |        219 mg       |    `1`    |  1 x Full Scale Accelerometer/2^6   |
|    `2`    |        250 mg       |    `2`    |  2 x Full Scale Accelerometer/2^6   |
|    `3`    |        312 mg       |    `3`    |  3 x Full Scale Accelerometer/2^6   |
|    `4`    |        344 mg       |   `...`   |        ...                          |
|    `5`    |        406 mg       |   `...`   |        ...                          |
|    `6`    |        469 mg       |   `...`   |        ...                          |
|    `7`    |        500 mg       |    `63`   |  63 x Full Scale Accelerometer/2^6  |


|  ID  |     Mode Name    |                Description                          |
|------|------------------|-----------------------------------------------------|
|  `0` | NONE             |   No data-ready interrupt                           |
|  `1` | Accel Data Ready |   Trigger when new Accelerometer data is available  |
|  `2` | Gyro Data Ready  |   Trigger when new Gyroscope data is available      |
|  `3` | Temp Data Ready  |   Trigger when new Temperature data is available    |
|  `4` | FIFO Threshold   |   Trigger when FIFO reaches the threshold level     |
|  `5` | FIFO Overrun     |   Trigger when FIFO is full and data is overwritten |
|  `6` | FIFO Full        |   Trigger when FIFO is completely filled            |
|  `7` | Counter BDR      |   Trigger based on Batch Data Rate counter          |


|  ID  |     Mode Name    |                Description                          |
|------|------------------|-----------------------------------------------------|
|  `0` | NONE             |   No motion event interrupt                         |
|  `1` | Sensor Hub       |   External sensor hub event                         |
|  `2` | Embedded Func    |   Generic embedded function event                   |
|  `3` | 6D Orientation   |   Trigger on change in device orientation           |
|  `4` | Double Tap       |   Trigger on double-tap detection                   |
|  `5` | Free-Fall        |   Trigger when free-fall is detected                |
|  `6` | Wake-Up          |   Trigger on wake-up motion                         |
|  `7` | Single Tap       |   Trigger on single-tap detection                   |
|  `8` | Sleep Change     |   Trigger when device enters or leaves sleep mode   |


## Recommended Execution Flow

### 1. Configuration Phase

Step 1: Configure Accelerometer

```text
lsm6d_conf_accel 5 3
```

Step 2: Configure Gyroscope

```text
lsm6d_conf_gyro 5 3
```

Step 3: Initialization

```text
lsm6d_init
```

### 2. Event Setup Phase (Optional)

Step 4: Event Configuration

```text
lsm6d_conf_wu 3 0
```
or free-fall event
```text
lsm6d_conf_ff 3 0
```

Step 5: Configure Interrupts

```text
lsm6d_conf_int 0 6
```

### 3. Data Acquisition Phase

Step 6: Read Data

```text
lsm6d_read_all
```
or specific read_accel/read_gyro commands

## Recommended command behavior

### `lsm6d_conf_accel`

Configure ODR and Full Scale Accelerometer , for example:

```text
lsm6d_conf_accel 5 3
```

```text
Configure Accelerometer: 208 Hz, 8 g
```

### `lsm6d_conf_gyro`

Configure ODR and Full Scale Gyroscope , for example:

```text
lsm6d_conf_gyro 5 3
```
```text
Configure Gyroscope:208 Hz, 1000 dps
```

### `lsm6d_conf_ff`
Sets Free-fall threshold and duration parameters , for example:

```text
lsm6d_conf_ff 3 1
```
```text
Success! Free-fall Configured:
Threshold: 312 mg
Duration: 1 x ODR_time
```

### `lsm6d_conf_wu`
Sets wake-up threshold and duration parameters , for example:

```text
lsm6d_conf_wu 3 1
```
```text
Success! Wake-up Configured:
Threshold: 3 x FS_XL/2^6
Duration: 1 x ODR_time
```
### `lsm6d_conf_int`

Configures Interrupt mode and event, for example:

```text
lsm6d_conf_int 6 0
```
```text
Success! INT1 Configured:
Mode: FIFO Full
Event: NO Event
```

### `lsm6d_init`
Initializes the LSM6DSOX, for example:

```text
lsm6d_init
```
```text
Init Success
```

### `lsm6d_dis_int`
Disables all active interrupts, for example:

```text
lsm6d_dis_int
```
```text
Disable Interrupt
```

### `lsm6d_check`
Verify LSM6DSOX is alive or not, for example:

```text
lsm6d_check
```
```text
LSM6DSOX Check OK
```

### `lsm6d_read_accel`
Reads and outputs 3-axis Accelerometer data (X, Y, Z) in mg, for example:

```text
lsm6d_read_accel
```
```text
Accel : X=-45.63 mg   Y=225.46 mg   Z=987.22 mg
```

### `lsm6d_read_gyro`
Reads and outputs 3-axis Gyroscope data (X, Y, Z) in dps, for example:

```text
lsm6d_read_gyro
```
```text
Gyro  : X=-0.67 dps   Y=-0.07 dps    Z=-0.21 dps
```

### `lsm6d_read_temp`
Reads and outputs the internal sensor temperature in Celsius, for example:

```text
lsm6d_read_temp
```
```text
Temperature: 30.85 C
```

### `lsm6d_read_all`
Reads and prints Accel (mg), Gyro (dps), and Temperature (°C) data, for example:

```text
lsm6d_read_all
```
```text
Gyro  : X=-0.52 dps   Y=-0.21 dps    Z=-0.25 dps
Accel : X=-42.94 mg   Y=225.46 mg   Z=1000.64 mg
Temp : 30.90 C
```