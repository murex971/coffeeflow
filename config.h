#ifndef COFFEEFLOW_CONFIG_H
#define COFFEEFLOW_CONFIG_H

// Pin map (ESP32 DevKit defaults — remap here after wiring)
static const int PIN_OLED_SDA = 21;
static const int PIN_OLED_SCL = 22;
static const int PIN_HX711_DT = 32;
static const int PIN_HX711_SCK = 33;
static const int PIN_BTN_TARE = 25;
static const int PIN_BTN_RUN = 26;

// OLED
static const int OLED_WIDTH = 128;
static const int OLED_HEIGHT = 64;
static const int OLED_I2C_ADDR = 0x3C;

// Scale: set SCALE_CAL with a known mass (see WIRING.md).
// Factor is "HX711 raw units per gram" after tare (bogde HX711 set_scale).
static const float SCALE_CAL = 420.0f;

static const int SCALE_SAMPLES = 4;
static const int SCALE_SMOOTH_N = 5;
static const unsigned long SCALE_READ_INTERVAL_MS = 100;  // ~10 Hz

// Buttons
static const unsigned long BTN_DEBOUNCE_MS = 40;
static const unsigned long BTN_LONG_PRESS_MS = 1000;

// UI
static const unsigned long UI_REFRESH_MS = 100;

// Flow: smooth live g/s over this window while Running
static const unsigned long FLOW_WINDOW_MS = 750;
static const float FLOW_MIN_DELTA_G = 0.3f;  // ignore tiny noise for live flow

#endif
