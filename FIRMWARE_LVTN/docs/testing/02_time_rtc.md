# RTC Test

## Expected coverage

- **RTC Ping** — verifies RTC communication via I2C read/write, returns OK if successful
- **RTC Check** — reads time twice with 1s interval and compares seconds, returns OK if RTC is alive

## Core rtc-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `rtc_ping` | none | return OK |
|  `rtc_test` | none | return OK |
|  `rtc_read` | none | return time |
| `rtc_set_date` | DD MM YY WD | RTC date set OK |
|  `rtc_set_time` | h m s | RTC time set OK |

## Recommended command behavior

### `rtc_ping`

Should return OK or FAIL

### `rtc_test`

Check rtc alive or not. Should return OK

### `rtc_set_date <DD> <MM> <YY> <WD>`

Set date for RTC, for example:

| Parameter | Range |     Description     |
|-----------|-------|---------------------|
| `DD`      | 1 – 31|        date         |
| `MM`      | 1 – 12|        month        |
| `YY`      | 0 – 99|        year         |
| `WD`      | 1 – 7 |        weekday      |

```text
rtc_set_date 7 4 26 3
```

### `rtc_set_time <h> <m> <s>`

Set time for RTC, for example:

| Parameter | Range |     Description     |
|-----------|-------|---------------------|
|  `h`      | 0 – 23|        hour         |
|  `m`      | 0 – 59|        minute       |
|  `s`      | 0 – 59|        second       |


```text
rtc_set_time 11 12 13
```

### `rtc_read`

Should return the time, for example:

```text
TIME: 11:12:13
Wed, 7/4/26
```