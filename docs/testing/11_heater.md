# Heater Module

## Expected coverage

- Control heater output using PWM

---

## Core commands

| Command | Arguments | Expected Response |
|---|---|---|
| heater_set | channel and duty | OK |

---

## Recommended command behavior

### `heater_set <channel> <duty>`

Set heater power (0–100%).

Example:

> heater_set 1 50

