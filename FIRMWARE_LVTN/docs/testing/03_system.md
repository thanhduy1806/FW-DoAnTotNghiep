# System Monitoring Module

## Expected coverage

- Monitor RTOS task status
- Inspect system logs for debugging
- Support basic system diagnostics

---

## Core commands

| Command | Arguments | Expected Response |
|---|---|---|
| os_stat | none | task list |
| dmesg | [N] | log output |

---

## Recommended command behavior

### `os_stat`

Display all running tasks in the system.

Information includes:

- Task name

- State (Running / Ready / Blocked)

- Stack usage

- CPU usage (if supported)

- Priority

---

### `dmesg [N]`

Display system logs.

- N (optional): number of recent log lines
