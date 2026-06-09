# Pump Test

## Expected coverage

- **PUMP Check Status** — enable pump and prompts tester to visually confirm, returns PASS if tester inputs YES

## Core pump-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `pump_enable` | none | Enable pump |
| `pump_disable` | none | Disable pump |
| `pump_set_volt` | Vpp | Set peak-to-peak voltage for pump |
| `pump_set_freq` | Hz | Set frequency for pump |

## Recommended command behavior

### `pump_enable`

Enable pump

### `pump_disable`

Disable pump

### `pump_set_volt <Vpp>`

Set peak-to-peak voltage for pump, for example:

```text
pump_set_volt 150
```

### `pump_set_freq <Hz>`

Set frequency for pump, for example:

```text
pump_set_freq 100
```