#ifndef __FILTER__
#define __FILTER__

#pragma once
#include "daisysp.h"

class EnvFilter {
public:
  enum FilterMode {
    FILTER_MODE_LOWPASS = 0,
    FILTER_MODE_HIGHPASS,
    FILTER_MODE_BANDPASS,
    kNumFilterModes
  };
  EnvFilter()
      : cutoff(0.99f), resonance(0.01f), mode(FILTER_MODE_LOWPASS), buf0(0.0f),
        buf1(0.0f), buf2(0.0f), buf3(0.0f), cutoffMod(0.0f) {
    calculateFeedbackAmount();
  }
  float Process(float inputValue);
  inline void setCutoff(float newCutoff) {
    cutoff = newCutoff;
    calculateFeedbackAmount();
  };
  inline void setResonance(float newResonance) {
    resonance = newResonance;
    calculateFeedbackAmount();
  };
  inline void setFilterMode(FilterMode newMode) { mode = newMode; }
  inline void setCutoffMod(float newCutoffMod) {
    cutoffMod = newCutoffMod;
    calculateFeedbackAmount();
  }
  void reset() { buf0 = buf1 = buf2 = buf3 = 0.0; }

private:
  float cutoff;
  float resonance;
  FilterMode mode;
  float feedbackAmount;
  float buf0;
  float buf1;
  float buf2;
  float buf3;
  float cutoffMod;
  inline float getCalculatedCutoff() const {
    return daisysp::fmax(daisysp::fmin(cutoff + cutoffMod, 0.99), 0.01);
  }
  inline void calculateFeedbackAmount() {
    feedbackAmount = resonance + resonance / (1.0 - getCalculatedCutoff());
  }
};

#endif
