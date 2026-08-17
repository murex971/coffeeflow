#ifndef COFFEEFLOW_BUTTONS_H
#define COFFEEFLOW_BUTTONS_H

#include <Arduino.h>

enum ButtonId {
  BTN_TARE = 0,
  BTN_RUN = 1,
};

enum ButtonEvent {
  BTN_NONE = 0,
  BTN_SHORT,
  BTN_LONG,
};

void buttonsBegin();
void buttonsUpdate();
ButtonEvent buttonsConsume(ButtonId id);

#endif
