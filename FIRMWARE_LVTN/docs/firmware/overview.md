# Firmware Overview

The firmware folder in this package is structured to support modular development.

## Included skeleton folders

- `src/`
- `inc/`
- `bsp/`
- `drivers/`
- `cli/`

## Expected coding style

- command handlers stay small
- board logic stays in BSP
- register details stay in drivers
- outputs for CLI commands remain deterministic
