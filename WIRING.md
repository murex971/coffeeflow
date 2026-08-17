# CoffeeFlow wiring & bring-up

Battery-powered pour-over scale: live weight, timer, flow rate; manual end shows time + average flow.

## Parts (Plan A)

| Part | Role |
|------|------|
| ESP32 DevKit | MCU |
| 0.96" OLED SSD1306 (I2C) | Display |
| 1 kg load cell + HX711 | Weight |
| 2× momentary buttons | Tare, Start/Pause/End |
| TP4056 + protected 3.7 V LiPo | Charge + cell |
| 5 V boost module (MT3608 or “UPS” charge+boost) | Power ESP32 `VIN`/`5V` from LiPo |

**Important:** Do not feed raw LiPo (~3.7 V) into the ESP32 `3V3` pin. Charge the cell with TP4056; boost to **5 V** and feed the DevKit **`5V` / `VIN`** pin (same as USB). USB alone is fine while developing.

## Pin map

| Function | ESP32 GPIO | Module |
|----------|------------|--------|
| OLED SDA | 21 | OLED SDA |
| OLED SCL | 22 | OLED SCL |
| OLED VCC | 3V3 | OLED VCC |
| OLED GND | GND | OLED GND |
| HX711 DT | 32 | HX711 DT / DOUT |
| HX711 SCK | 33 | HX711 SCK |
| HX711 VCC | 3V3 (or 5V if module expects 5V) | HX711 VCC |
| HX711 GND | GND | GND |
| Tare button | 25 | to GND (internal pull-up) |
| Run button | 26 | to GND (internal pull-up) |

Load cell → HX711: follow the silk on your HX711 board (E+, E−, A+, A−). Red/black/white/green wiring varies by cell; if weight goes negative when you add mass, swap A+ and A− or invert calibration sign.

All pins are defined in [`config.h`](config.h).

## Button map

| Button | Action |
|--------|--------|
| **Tare** short | Zero scale (Idle / Paused / Done). Ignored while Running. |
| **Run** short | Idle→start; Running↔Paused; Done→Idle reset |
| **Run** long (~1 s) | Manual end → summary (time + avg g/s) |

## Arduino IDE setup

1. Install **Arduino IDE 2.x**.
2. Boards Manager: install **esp32** by Espressif (`esp32` board package).
3. Board: **ESP32 Dev Module**, upload speed **115200**.
4. Library Manager — install:
   - **HX711** by Bogdan Necula (or “HX711 Arduino Library”)
   - **Adafruit SSD1306**
   - **Adafruit GFX Library**
5. Open [`coffeeflow.ino`](coffeeflow.ino) from this folder (folder name must stay `coffeeflow`).
6. Select the serial port, Upload.

## Calibration (`SCALE_CAL`)

1. Power with USB. Open Serial Monitor at **115200**.
2. Empty the platform; press **Tare** (or reboot — firmware tares once after boot).
3. Place a known mass (e.g. 100 g calibration weight or kitchen-scale-checked object).
4. Note Serial `g=` reading.
5. New factor:

   `SCALE_CAL_new = SCALE_CAL_old × (reading_g / known_g)`

6. Edit `SCALE_CAL` in [`config.h`](config.h), re-upload.
7. Confirm reading is within about **±1 g** with a mug.

If readings are unstable, check mechanical mounting, short HX711 wires, and shared GND.

## Bring-up order

1. USB only: OLED shows weight after boot.
2. Confirm Serial `g=` / `raw=` change when you press the cell.
3. Calibrate `SCALE_CAL`.
4. Verify Tare, Start/Pause, long-press End, Done reset.
5. Connect LiPo → TP4056 → **5 V boost** → ESP32 `5V`/`VIN`; unplug USB and retest.

## Firmware layout

| File | Role |
|------|------|
| `coffeeflow.ino` | `setup` / `loop` |
| `config.h` | Pins, timings, `SCALE_CAL` |
| `scale.*` | HX711 + smoothing |
| `buttons.*` | Debounce, short/long |
| `brew.*` | Idle / Running / Paused / Done |
| `ui.*` | SSD1306 screens |

## Out of scope (v1)

Wi‑Fi/app, auto-end, battery %, recipes, SD logging.
