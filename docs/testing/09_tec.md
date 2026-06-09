# TEC Test

## Expected coverage

- **TEC Check** — sends CLI command to initialize TEC, returns PASS if terminal responds success

## Core tec-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `tec_init` | index | return success or fail |
| `tec_vol` | index mVol | set voltage output |

## Recommended command behavior

| Parameter | Range |     Description     |
|-----------|-------|---------------------|
|  `index`  | 1 – 4 |       index         |

### `tec_init <index>`

Initialize the TEC controller at the specified index, for example:

```text
tec_init 1
```

### `tec_vol <index> <mVol>`

Set voltage output for the specified TEC, for example:

```text
tec_vol 1 1000
```