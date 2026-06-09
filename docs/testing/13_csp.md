# CSP Module

## Expected coverage

- Test communication between nodes
- Send/receive data

---

## Core commands

| Command | Arguments | Expected Response |
|---|---|---|
| csp_ping | addr and timeout | Reply from node : [time] ms |
| csp_send | addr, port and data | |
| csp_info | none | status |

---

## Recommended command behavior

### `csp_ping <addr> <timeout>`
Check node connectivity.

---

### `csp_send <addr> <port> <data>`
Send data.

---

### `csp_info`
Return information of node.