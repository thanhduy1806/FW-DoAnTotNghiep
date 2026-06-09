# Robot Framework

The included smoke suite demonstrates the intended command-testing style.

## Important parser reminder

Robot splits arguments by two or more spaces or tabs.

So this line:

```robot
Execute Command And Expect    echo hello    hello
```

passes:

- `echo hello` as the first argument
- `hello` as the second argument

## Reusable keyword examples

- `Ping Should Return Pong`
- `Version Should Be Printed`
- `Echo Should Return Argument`
