# ATSAMV71 REPL

The included `atsamv71.repl` file is a practical starting point for the project.

## Included assumptions

- the platform is named `samv71`
- `USART0` is available and used as the console
- memory regions are declared for firmware loading
- additional peripherals can be modeled later as needed

## Recommended usage policy

- do not redeclare a peripheral that is already provided by a reused base platform
- add dummy or partial models for SPI and I2C only when firmware starts to access them
- document every gap that may affect simulation results
