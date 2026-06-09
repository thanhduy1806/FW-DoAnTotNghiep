# Board Assumptions

This package uses early-stage assumptions suitable for a bring-up project.

## Current assumptions

- target MCU is ATSAMV71
- primary console is `USART0`
- clock and reset details are still subject to board revision confirmation
- some external peripherals may not yet be validated in simulation
- simulation is initially focused on CLI and boot observability

## What should be confirmed later

- exact oscillator and clock tree
- actual console pin routing
- boot source and debug method
- external SPI and I2C devices
- LEDs or control GPIOs used during bring-up
