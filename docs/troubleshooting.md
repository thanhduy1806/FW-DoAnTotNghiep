# Troubleshooting

## No console output in Renode

Check:

- `firmware.elf` path
- REPL console peripheral
- `showAnalyzer usart0` or equivalent console setup
- whether the firmware actually initializes the console

## Robot test does not see expected response

Check:

- line endings
- timeout value
- prompt noise
- whether the command response changed without updating the tests

## Simulation hangs after enabling SPI or I2C

Likely causes:

- unmodeled peripheral access
- blocking polling loop
- interrupt expected but never generated
