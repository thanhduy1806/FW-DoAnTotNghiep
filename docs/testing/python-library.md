# Python Library

The project includes a small Python library skeleton:

```text
tests/python_lib/serial_cli.py
```

## Purpose

It provides a transport layer for Robot keywords such as:

- `Execute Command And Expect`

## Main method idea

```python
def execute_command_and_expect(self, command, expected, timeout_s=3.0, retries=None):
    ...
```

## Interpretation

- `command` is the full CLI command string, such as `echo hello`
- `expected` is the response fragment, such as `hello`
- the method sends the command and waits for the expected text
