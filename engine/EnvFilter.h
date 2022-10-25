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
  void SetCutoff(float newCutoff);
  void SetResonance(float newResonance);
  void SetFilterMode(FilterMode newMode);
  void SetCutoffMod(float newCutoffMod);
  void Reset();

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
