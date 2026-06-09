# Bring-up Checklist

## Console

- `USART0` pins confirmed
- baud rate documented
- boot banner visible
- prompt visible
- line endings handled correctly

## Basic firmware

- no immediate hard fault
- firmware reaches main loop
- CLI remains responsive after several commands

## Peripheral sanity

- GPIO toggle path reviewed
- SPI init path does not hang
- I2C init path has timeout handling
- timer use does not create infinite waits in simulation
