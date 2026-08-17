#ifndef COFFEEFLOW_BREW_H
#define COFFEEFLOW_BREW_H

#include <Arduino.h>

enum BrewState {
  BREW_IDLE = 0,
  BREW_RUNNING,
  BREW_PAUSED,
  BREW_DONE,
};

void brewBegin();
void brewUpdate(float grams);
void brewOnTareShort();
void brewOnRunShort();
void brewOnRunLong();

BrewState brewState();
unsigned long brewElapsedMs();
float brewLiveFlowGps();
float brewAvgFlowGps();
bool brewHasLiveFlow();
bool brewHasAvgFlow();

#endif
