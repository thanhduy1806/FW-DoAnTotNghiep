# CLI Commands

This package already assumes a starter command set for smoke testing.

## Core smoke-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `help` | none | prints command list |
| `version` | none | prints firmware version |
| `ping` | none | prints `PONG` |
| `echo <text>` | text | prints the same text |
| `selftest` | none | prints pass or summary |
| `reboot` | none | prints reboot message or simulated reboot notice |

## Recommended command behavior

### `help`

Should list supported commands and optionally short usage text.

### `version`

Should print a stable version string such as:

```text
ATSAMV71-RENODE-0.1.0
```

### `ping`

Should return exactly:

```text
PONG
```

### `echo <text>`

Should return exactly the input text.

### `selftest`

Should be deterministic and script-friendly, for example:

```text
SELFTEST: PASS
```

### `reboot`

In simulation, it may print a placeholder message such as:

```text
Reboot requested (simulation only)
```
