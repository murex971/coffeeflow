#include "brew.h"
#include "config.h"
#include "scale.h"

static BrewState state = BREW_IDLE;
static unsigned long elapsedMs = 0;
static unsigned long runSegmentStartMs = 0;
static bool segmentOpen = false;

static float liveFlowGps = 0.0f;
static bool hasLiveFlow = false;
static float avgFlowGps = 0.0f;
static bool hasAvgFlow = false;
static float endWeightG = 0.0f;

static float flowWeightAnchor = 0.0f;
static unsigned long flowTimeAnchorMs = 0;

static void openRunSegment() {
  runSegmentStartMs = millis();
  segmentOpen = true;
  flowWeightAnchor = scaleGrams();
  flowTimeAnchorMs = millis();
  hasLiveFlow = false;
  liveFlowGps = 0.0f;
}

static void closeRunSegment() {
  if (segmentOpen) {
    elapsedMs += millis() - runSegmentStartMs;
    segmentOpen = false;
  }
  hasLiveFlow = false;
}

static void resetSession() {
  closeRunSegment();
  elapsedMs = 0;
  liveFlowGps = 0.0f;
  hasLiveFlow = false;
  avgFlowGps = 0.0f;
  hasAvgFlow = false;
  endWeightG = 0.0f;
  flowWeightAnchor = 0.0f;
  flowTimeAnchorMs = 0;
}

static void enterDone(float grams) {
  closeRunSegment();
  endWeightG = grams;
  if (elapsedMs > 0 && endWeightG > 0.05f) {
    avgFlowGps = endWeightG / (elapsedMs / 1000.0f);
    hasAvgFlow = true;
  } else {
    avgFlowGps = 0.0f;
    hasAvgFlow = false;
  }
  hasLiveFlow = false;
  state = BREW_DONE;
}

void brewBegin() {
  state = BREW_IDLE;
  resetSession();
}

void brewUpdate(float grams) {
  if (state == BREW_RUNNING && segmentOpen) {
    unsigned long now = millis();
    unsigned long dt = now - flowTimeAnchorMs;
    if (dt >= FLOW_WINDOW_MS) {
      float dw = grams - flowWeightAnchor;
      if (dw >= FLOW_MIN_DELTA_G) {
        liveFlowGps = dw / (dt / 1000.0f);
        hasLiveFlow = true;
      } else {
        hasLiveFlow = false;
        liveFlowGps = 0.0f;
      }
      flowWeightAnchor = grams;
      flowTimeAnchorMs = now;
    }
  }
}

void brewOnTareShort() {
  if (state == BREW_RUNNING) {
    return;
  }
  scaleTare();
  if (state == BREW_DONE) {
    // Stay on Done; tare only zeros the scale for next brew prep.
  }
}

void brewOnRunShort() {
  switch (state) {
    case BREW_IDLE:
      resetSession();
      state = BREW_RUNNING;
      openRunSegment();
      break;
    case BREW_RUNNING:
      closeRunSegment();
      state = BREW_PAUSED;
      break;
    case BREW_PAUSED:
      state = BREW_RUNNING;
      openRunSegment();
      break;
    case BREW_DONE:
      resetSession();
      state = BREW_IDLE;
      break;
  }
}

void brewOnRunLong() {
  if (state == BREW_RUNNING || state == BREW_PAUSED) {
    enterDone(scaleGrams());
  }
}

BrewState brewState() {
  return state;
}

unsigned long brewElapsedMs() {
  unsigned long total = elapsedMs;
  if (state == BREW_RUNNING && segmentOpen) {
    total += millis() - runSegmentStartMs;
  }
  return total;
}

float brewLiveFlowGps() {
  return liveFlowGps;
}

float brewAvgFlowGps() {
  return avgFlowGps;
}

bool brewHasLiveFlow() {
  return hasLiveFlow;
}

bool brewHasAvgFlow() {
  return hasAvgFlow;
}
