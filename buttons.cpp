#include "buttons.h"
#include "config.h"

struct BtnState {
  int pin;
  bool stableDown;
  bool lastRawDown;
  unsigned long lastChangeMs;
  unsigned long downSinceMs;
  bool longFired;
  ButtonEvent pending;
};

static BtnState btns[2];

static void initBtn(BtnState &b, int pin) {
  b.pin = pin;
  b.stableDown = false;
  b.lastRawDown = false;
  b.lastChangeMs = 0;
  b.downSinceMs = 0;
  b.longFired = false;
  b.pending = BTN_NONE;
  pinMode(pin, INPUT_PULLUP);
}

void buttonsBegin() {
  initBtn(btns[BTN_TARE], PIN_BTN_TARE);
  initBtn(btns[BTN_RUN], PIN_BTN_RUN);
}

static void pollOne(BtnState &b) {
  bool rawDown = digitalRead(b.pin) == LOW;
  unsigned long now = millis();

  if (rawDown != b.lastRawDown) {
    b.lastRawDown = rawDown;
    b.lastChangeMs = now;
  }

  if ((now - b.lastChangeMs) < BTN_DEBOUNCE_MS) {
    return;
  }

  if (rawDown && !b.stableDown) {
    b.stableDown = true;
    b.downSinceMs = now;
    b.longFired = false;
  } else if (!rawDown && b.stableDown) {
    b.stableDown = false;
    if (!b.longFired) {
      unsigned long held = now - b.downSinceMs;
      if (held >= BTN_LONG_PRESS_MS) {
        b.pending = BTN_LONG;
      } else {
        b.pending = BTN_SHORT;
      }
    }
  } else if (rawDown && b.stableDown && !b.longFired) {
    if ((now - b.downSinceMs) >= BTN_LONG_PRESS_MS) {
      b.longFired = true;
      b.pending = BTN_LONG;
    }
  }
}

void buttonsUpdate() {
  pollOne(btns[BTN_TARE]);
  pollOne(btns[BTN_RUN]);
}

ButtonEvent buttonsConsume(ButtonId id) {
  ButtonEvent e = btns[id].pending;
  btns[id].pending = BTN_NONE;
  return e;
}
