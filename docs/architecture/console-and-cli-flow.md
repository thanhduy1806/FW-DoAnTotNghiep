# Console and CLI Flow

## Default console choice

This package assumes:

- `USART0` is the primary console
- it is used for manual interaction and Robot automation
- all smoke tests are based on stable CLI outputs

## Typical flow

1. firmware boots
2. `USART0` becomes active
3. boot banner is printed
4. CLI prompt appears
5. user or Robot sends commands
6. command handler executes
7. response is printed back on the console

## Recommended prompt policy

Use a consistent prompt such as:

```text
>
```

or

```text
cli>
```

and keep it stable so tests stay simple.
