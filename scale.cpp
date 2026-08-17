#include "scale.h"
#include "config.h"

#include <HX711.h>

static HX711 hx;
static float smoothBuf[SCALE_SMOOTH_N];
static int smoothIdx = 0;
static int smoothCount = 0;
static float gramsSmoothed = 0.0f;
static unsigned long lastReadMs = 0;
static long lastRaw = 0;

void scaleBegin() {
  hx.begin(PIN_HX711_DT, PIN_HX711_SCK);
  hx.set_scale(SCALE_CAL);
  hx.set_offset(0);
  for (int i = 0; i < SCALE_SMOOTH_N; i++) {
    smoothBuf[i] = 0.0f;
  }
  smoothIdx = 0;
  smoothCount = 0;
  gramsSmoothed = 0.0f;
  lastReadMs = 0;

  // Wait for HX711 ready (USB bring-up can take a moment)
  unsigned long start = millis();
  while (!hx.is_ready() && (millis() - start) < 2000) {
    delay(10);
  }
}

void scaleTare() {
  hx.tare(SCALE_SAMPLES);
  for (int i = 0; i < SCALE_SMOOTH_N; i++) {
    smoothBuf[i] = 0.0f;
  }
  smoothIdx = 0;
  smoothCount = 0;
  gramsSmoothed = 0.0f;
}

void scaleUpdate() {
  unsigned long now = millis();
  if (lastReadMs != 0 && (now - lastReadMs) < SCALE_READ_INTERVAL_MS) {
    return;
  }
  lastReadMs = now;

  if (!hx.is_ready()) {
    return;
  }

  // Single conversion: get_value is (raw average - offset); set_scale uses SCALE_CAL
  float value = hx.get_value(SCALE_SAMPLES);
  lastRaw = (long)value;
  float g = value / SCALE_CAL;

  smoothBuf[smoothIdx] = g;
  smoothIdx = (smoothIdx + 1) % SCALE_SMOOTH_N;
  if (smoothCount < SCALE_SMOOTH_N) {
    smoothCount++;
  }

  float sum = 0.0f;
  for (int i = 0; i < smoothCount; i++) {
    sum += smoothBuf[i];
  }
  gramsSmoothed = sum / (float)smoothCount;
}

float scaleGrams() {
  return gramsSmoothed;
}

long scaleRaw() {
  return lastRaw;
}
