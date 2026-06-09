# Peripheral Plan

## Current engineering priority

The first peripherals to stabilize are:

1. `USART0` for CLI
2. PIO/GPIO for simple visibility
3. SPI for bring-up path review
4. I2C/TWI for device enumeration or RTC-style commands
5. timer functionality if CLI or services rely on it

## Status model

| Peripheral | Role | Firmware Expectation | Simulation Expectation |
|---|---|---|---|
| USART0 | console | active and stable | active and visible |
| GPIO/PIO | LEDs or control | basic toggling | partial or observable |
| SPI0 | command-level bring-up | init path first | partial, stub, or planned |
| TWI0 | simple access commands | timeout-safe | partial, stub, or planned |
| Timer | delays and timestamps | bounded behavior | may differ from hardware |

## Design warning

If firmware accesses a peripheral that is not modeled in Renode, simulation may:

- log unhandled reads or writes
- hang in polling loops
- never trigger expected interrupts
