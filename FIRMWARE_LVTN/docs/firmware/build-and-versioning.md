# Build and Versioning

## Build output

The current package assumes the main build artifact is:

```text
firmware/build/firmware.elf
```

## Version policy

A version command is useful for both manual review and automation.

Recommended format:

```text
ATSAMV71-RENODE-<major>.<minor>.<patch>
```

Example:

```text
ATSAMV71-RENODE-0.1.0
```

## Documentation rule

Whenever the visible command set or versioning approach changes, update:

- this page
- the CLI commands page
- Robot smoke tests if expected outputs change
