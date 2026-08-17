#ifndef COFFEEFLOW_UI_H
#define COFFEEFLOW_UI_H

#include <Arduino.h>
#include "brew.h"

void uiBegin();
void uiRender(float grams, BrewState state, unsigned long elapsedMs,
              float liveFlow, bool hasLiveFlow, float avgFlow,
              bool hasAvgFlow, bool force);

#endif
