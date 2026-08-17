#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "scale.h"
#include "buttons.h"
#include "brew.h"
#include "ui.h"

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("CoffeeFlow"));

  WiFi.mode(WIFI_OFF);
  btStop();

  scaleBegin();
  buttonsBegin();
  brewBegin();
  uiBegin();

  // Initial tare with empty platform recommended after boot settle
  delay(500);
  scaleTare();

  uiRender(scaleGrams(), brewState(), brewElapsedMs(), brewLiveFlowGps(),
           brewHasLiveFlow(), brewAvgFlowGps(), brewHasAvgFlow(), true);

  Serial.println(F("Ready. Tare=GPIO25  Run=GPIO26 (long=end)"));
  Serial.println(F("Calibrate SCALE_CAL in config.h — see WIRING.md"));
}

void loop() {
  buttonsUpdate();
  scaleUpdate();

  ButtonEvent tareEv = buttonsConsume(BTN_TARE);
  if (tareEv == BTN_SHORT) {
    brewOnTareShort();
  }

  ButtonEvent runEv = buttonsConsume(BTN_RUN);
  if (runEv == BTN_SHORT) {
    brewOnRunShort();
  } else if (runEv == BTN_LONG) {
    brewOnRunLong();
  }

  float g = scaleGrams();
  brewUpdate(g);

  uiRender(g, brewState(), brewElapsedMs(), brewLiveFlowGps(),
           brewHasLiveFlow(), brewAvgFlowGps(), brewHasAvgFlow(), false);

  // Occasional raw dump helps calibration over Serial Monitor
  static unsigned long lastLog = 0;
  unsigned long now = millis();
  if (now - lastLog >= 1000) {
    lastLog = now;
    Serial.print(F("g="));
    Serial.print(g, 2);
    Serial.print(F(" raw="));
    Serial.print(scaleRaw());
    Serial.print(F(" state="));
    Serial.println((int)brewState());
  }
}
