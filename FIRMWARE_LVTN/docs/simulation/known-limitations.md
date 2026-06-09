# Known Limitations

## Current expected gaps

At this stage, simulation is expected to be strongest for:

- boot path
- CLI behavior
- `USART0` console interaction

Simulation is expected to be weaker for:

- exact SPI behavior
- exact I2C/TWI status modeling
- timing-sensitive interrupt paths
- hardware-specific edge cases

## Engineering rule

When simulation and hardware disagree, the difference must be recorded here and later classified as:

- firmware defect
- simulation limitation
- documentation mismatch
