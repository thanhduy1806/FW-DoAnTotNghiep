# CAN Module

## Expected coverage

- Send raw CAN messages

---

## Core commands

| Command | Arguments | Expected Response |
|---|---|---|
| can_send | id [ext] data | OK |

---

## Recommended command behavior

### `can_send <id> [ext] [data]`

Send CAN frame.

Example:
>can_send 0x123 01 02 03
