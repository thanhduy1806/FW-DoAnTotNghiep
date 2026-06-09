# HIL Transition Plan

## Why transition to HIL

Simulation is excellent for console-driven bring-up, but hardware-in-the-loop is needed for:

- real SPI transfers
- real I2C behavior
- actual timing and interrupts
- board-specific pin behavior

## Transition order

1. keep CLI identical across simulation and hardware
2. reuse the same Robot testing philosophy if possible
3. move peripheral-sensitive cases to hardware once simulation stops being sufficient
