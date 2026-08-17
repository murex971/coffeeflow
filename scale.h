#ifndef COFFEEFLOW_SCALE_H
#define COFFEEFLOW_SCALE_H

#include <Arduino.h>

void scaleBegin();
void scaleUpdate();
void scaleTare();
float scaleGrams();
long scaleRaw();

#endif
