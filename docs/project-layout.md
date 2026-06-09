# Project Layout

The package already includes a recommended repository skeleton.

```text
project-root/
├─ mkdocs.yml
├─ README.md
├─ docs/
├─ firmware/
│  ├─ src/
│  ├─ inc/
│  ├─ bsp/
│  ├─ drivers/
│  ├─ cli/
│  ├─ linker.ld
│  └─ Makefile
├─ renode/
│  ├─ atsamv71.repl
│  ├─ run.resc
│  └─ test.resc
├─ tests/
│  ├─ robot/
│  │  └─ smoke.robot
│  └─ python_lib/
│     └─ serial_cli.py
└─ .github/
   └─ workflows/
      └─ ci.yml
```

## Folder intent

- `firmware/`: embedded source, linker, build rules
- `renode/`: simulation platform and scripts
- `tests/robot/`: Robot Framework suites
- `tests/python_lib/`: Robot support library
- `docs/`: project documentation
