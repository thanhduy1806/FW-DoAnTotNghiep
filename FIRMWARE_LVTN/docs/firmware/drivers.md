# Drivers

## Target driver set

- UART driver for `USART0`
- GPIO/PIO driver
- SPI driver
- I2C/TWI driver
- optional timer driver

## Driver guidance

- keep APIs explicit
- document timeout policy
- avoid open-ended polling loops where possible
- identify which behavior is expected to work in Renode and which must be confirmed on hardware
