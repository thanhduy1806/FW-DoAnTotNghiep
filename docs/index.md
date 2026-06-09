# ATSAMV71 Firmware Project

This documentation package is tailored for an ATSAMV71 firmware project that uses:

- `USART0` as the primary CLI console
- Renode for simulation-based bring-up
- Robot Framework for automated CLI verification
- a Python transport library for command execution
- CI/CD for repeatable build and validation

## Main objectives

The documentation is intended to make the project easier to:

- build
- understand
- simulate
- test
- maintain

## What is already assumed in this package

This package already assumes a practical early-stage setup:

- console is routed through `USART0`
- Renode may reuse a platform close to ATSAMV71 for early development
- CLI is the main interface for smoke tests
- some peripherals may initially be stubbed or partially modeled
- simulation and hardware verification will coexist during bring-up

## Recommended reading order

1. Getting Started
2. Project Layout
3. Architecture Overview
4. Board Assumptions
5. Firmware CLI Commands
6. ATSAMV71 REPL
7. Smoke Test Suite
8. Pipeline
