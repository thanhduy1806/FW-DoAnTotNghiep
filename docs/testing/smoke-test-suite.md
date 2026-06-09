# Smoke Test Suite

The included Robot suite focuses on the starter CLI commands.

## Expected coverage

- prompt-related startup validation may be done manually at first
- `help` returns a command list
- `version` returns a stable version string
- `ping` returns `PONG`
- `echo test123` returns `test123`

## Purpose

These tests provide a fast confidence check after each build or simulation change.
