# Dual Solenoid Test

## Expected coverage

- **Solenoid Check** — opens solenoid and prompts tester to visually confirm, returns PASS if tester inputs YES

## Core dual-solenoid-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `sol_dual_forward` | index | Change direction of specific solenoid |
| `sol_dual_reverse` | index | Change direction of specific solenoid |

## Recommended command behavior

| Parameter | Range |     Description     |
|-----------|-------|---------------------|
|  `index`  | 1 – 4 |       index         |

### `sol_dual_forward <index>`

Switch on the solenoid, for example:

```text
sol_dual_forward 1
```

### `sol_dual_reverse <index>`

Switch off the solenoid, for example:

```text
sol_dual_reverse 1
```