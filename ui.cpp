#include "ui.h"
#include "config.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
static unsigned long lastDrawMs = 0;
static bool ready = false;

static void formatTime(unsigned long ms, char *buf, size_t buflen) {
  unsigned long totalSec = ms / 1000UL;
  unsigned long minutes = totalSec / 60UL;
  unsigned long seconds = totalSec % 60UL;
  if (minutes > 99) {
    minutes = 99;
  }
  snprintf(buf, buflen, "%lu:%02lu", minutes, seconds);
}

void uiBegin() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  ready = display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR);
  if (!ready) {
    Serial.println(F("SSD1306 init failed"));
    return;
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("CoffeeFlow"));
  display.println(F("booting..."));
  display.display();
  lastDrawMs = 0;
}

static void drawBrew(float grams, BrewState state, unsigned long elapsedMs,
                     float liveFlow, bool hasLiveFlow) {
  char line[24];
  char tbuf[8];

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 0);
  snprintf(line, sizeof(line), "%5.1f g", grams);
  display.print(line);

  formatTime(elapsedMs, tbuf, sizeof(tbuf));
  display.setTextSize(2);
  display.setCursor(0, 24);
  display.print(tbuf);

  display.setTextSize(1);
  display.setCursor(0, 48);
  if (state == BREW_PAUSED) {
    display.print(F("PAUSED"));
  } else if (state == BREW_RUNNING && hasLiveFlow) {
    snprintf(line, sizeof(line), "%.1f g/s", liveFlow);
    display.print(line);
  } else if (state == BREW_IDLE) {
    display.print(F("Run: start"));
  } else {
    display.print(F("-- g/s"));
  }

  display.display();
}

static void drawDone(unsigned long elapsedMs, float avgFlow, bool hasAvgFlow) {
  char line[24];
  char tbuf[8];

  display.clearDisplay();
  formatTime(elapsedMs, tbuf, sizeof(tbuf));

  display.setTextSize(2);
  display.setCursor(0, 4);
  display.print(tbuf);

  display.setTextSize(2);
  display.setCursor(0, 28);
  if (hasAvgFlow) {
    snprintf(line, sizeof(line), "%.1f g/s", avgFlow);
    display.print(line);
  } else {
    display.print(F("-- g/s"));
  }

  display.setTextSize(1);
  display.setCursor(0, 54);
  display.print(F("avg  |  Run: reset"));

  display.display();
}

void uiRender(float grams, BrewState state, unsigned long elapsedMs,
              float liveFlow, bool hasLiveFlow, float avgFlow, bool hasAvgFlow,
              bool force) {
  if (!ready) {
    return;
  }

  unsigned long now = millis();
  if (!force && lastDrawMs != 0 && (now - lastDrawMs) < UI_REFRESH_MS) {
    return;
  }
  lastDrawMs = now;

  if (state == BREW_DONE) {
    drawDone(elapsedMs, avgFlow, hasAvgFlow);
  } else {
    drawBrew(grams, state, elapsedMs, liveFlow, hasLiveFlow);
  }
}
