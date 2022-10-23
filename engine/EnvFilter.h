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
    CalculateFeedbackAmount();
  }
  float Process(float inputValue);
  inline void SetCutoff(float newCutoff) {
    cutoff = newCutoff;
    CalculateFeedbackAmount();
  };
  inline void SetResonance(float newResonance) {
    resonance = newResonance;
    CalculateFeedbackAmount();
  };
  inline void SetFilterMode(FilterMode newMode) { mode = newMode; }
  inline void SetCutoffMod(float newCutoffMod) {
    cutoffMod = newCutoffMod;
    CalculateFeedbackAmount();
  }
  void Reset() { buf0 = buf1 = buf2 = buf3 = 0.0; }

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
  inline float GetCalculatedCutoff() const {
    return daisysp::fmax(daisysp::fmin(cutoff + cutoffMod, 0.99), 0.01);
  }
  inline void CalculateFeedbackAmount() {
    feedbackAmount = resonance + resonance / (1.0 - GetCalculatedCutoff());
  }
};

#endif
