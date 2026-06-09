# BSP

## BSP responsibilities

The BSP should own:

- clock init
- pin muxing
- console selection
- board-level reset behavior
- bring-up feature switches

## Current project assumption

This package assumes `USART0` is selected as the board console in the BSP.
That choice should appear clearly in the BSP code and in the board documentation.
