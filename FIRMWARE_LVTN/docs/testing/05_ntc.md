# NTC Test

## Expected coverage

- **NTC Check** — returns OK if all NTCs are working

## Core ntc-test commands

| Command | Arguments | Expected Response |
|---|---|---|
| `ntc_get_all` | none | return temperatures of all NTCs |
| `ntc_read <index>` | index | returns the temperature of the specified NTC |
| `ntc_check` | none | prints pass or summary |

## Recommended command behavior

### `ntc_get_all`

Should return temperatures of all NTCs.

### `ntc_read <index>`

Should return the temperature of the specified NTC(`index`: 1 – 8), for example:

```text
ntc_read 1
```

```text
NTC[1]: 253
```

### `ntc_check`

Should return OK if all NTCs are working or return which is not working