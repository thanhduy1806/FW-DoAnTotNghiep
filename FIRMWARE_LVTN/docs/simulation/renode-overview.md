# Renode Overview

Renode is used here to validate:

- boot and console behavior
- CLI command flow
- repeatable smoke tests in CI

## Current simulation focus

The package is intentionally centered on `USART0` and command-based testing because that is the most robust early-stage simulation path.
