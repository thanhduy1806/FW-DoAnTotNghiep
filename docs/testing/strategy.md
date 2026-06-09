# Testing Strategy

## Main levels

1. CLI smoke tests in simulation
2. regression checks in CI
3. board-level verification later in HIL

## Immediate scope

The package is ready to smoke-test:

- `help`
- `version`
- `ping`
- `echo`

Additional tests can be added later for:

- `selftest`
- GPIO-visible behavior
- SPI or I2C command wrappers
