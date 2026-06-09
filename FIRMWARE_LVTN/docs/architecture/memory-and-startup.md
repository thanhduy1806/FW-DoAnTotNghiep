# Memory and Startup

## Default project assumptions

These values are placeholders and should be aligned with your actual linker script and board setup.

| Region | Base | Size | Notes |
|---|---:|---:|---|
| FLASH | `0x00400000` | `2048 KB` | main firmware image |
| SRAM | `0x20400000` | `384 KB` | runtime data and stacks |

## Startup expectations

The startup path should:

1. set stack pointer
2. initialize vector table
3. initialize `.data`
4. clear `.bss`
5. run board-level init
6. start the CLI-capable application

## Documentation note

When the startup code, linker script, or memory layout changes, update both:

- this documentation page
- the Renode platform assumptions
