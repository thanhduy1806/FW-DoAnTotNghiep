# Single Solenoid Test

## Expected coverage

- **Expander Write And Read Check** — writes and reads back value at the same register address, returns PASS if values match
- **Expander Set Solenoid Check** — sends CLI command to turn ON solenoid, returns PASS if terminal responds OK
- **Expander Clear Solenoid Check** — sends CLI command to turn OFF solenoid, returns PASS if terminal responds OK

## Core single-solenoid-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `sol_single_on` | index | turn on index pin |
  `sol_single_off` | index | turn off index pin |
  `sol_single_get` | index | returns status of index pin |

*Note: index is from 1 - 16*

## Recommended command behavior

| Parameter | Range |     Description     |
|-----------|-------|---------------------|
|  `index`  | 1 – 16|       index         |


### `sol_single_on <index>`

Turn on specific pin, for example:

```text
sol_single_on 9
```

### `sol_single_off <index>`

Turn off specific pin, for example:

```text
sol_single_off 9
```

### `sol_single_get <index>`

Should return the status of the specified pin, for example:

```text
sol_single_get 9
```

```text
Status of single solenoid 9: OFF
```