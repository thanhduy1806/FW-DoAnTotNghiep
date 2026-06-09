# Power Control Module

Controls power supply (power rails) to all hardware modules.

---

## Expected coverage

- Turn ON/OFF all power rails
- Control power for individual modules
- Monitor power status
- Ensure correct power sequence

---

## Core commands

| Command | Arguments | Expected Response |
|---|---|---|
| power_all_on | none | status |
| power_all_off | none | status |
| power_all_get | none | status |
| power_som_on/off/get | none | status |
| power_buck_peri_on/off/get | none | status |
| power_tec_on/off/get | none | status |
| power_hd4_on/off/get | none | status |
| power_solenoid_on/off/get | none | status |
| power_lp_on/off/get | none | status |
| power_heater_on/off/get | none | status |

---

## Recommended command behavior

### `power_all_on`

Turn ON all power rails in the system.

---

### `power_all_off`

Turn OFF all power rails.

---

### `power_all_get`

Display status of all power rails.


---

## Individual power control

### `power_som_on / power_som_off / power_som_get`
Control power for System-On-Module (main processor).

---

### `power_buck_peri_on / off / get`
Control peripheral power rail.

---

### `power_tec_on / off / get`
Control TEC (Thermoelectric Cooler) power.

---

### `power_hd4_on / off / get`
Control HD4 module power.


---

### `power_solenoid_on / off / get`

Control solenoid power supply.

---

### `power_lp_on / off / get`

Control lp module.

---

### `power_heater_on / off / get`

Control heater power.

---
