#ifndef __FILTER__
#define __FILTER__

#pragma once
#include <arm_math.h>

class Filter {
public:
  enum FilterMode {
    FILTER_MODE_LOWPASS = 0,
    FILTER_MODE_HIGHPASS,
    FILTER_MODE_BANDPASS,
    kNumFilterModes
  };
  Filter()
      : cutoff(0.99), resonance(0.01), mode(FILTER_MODE_LOWPASS), buf0(0.0),
        buf1(0.0), buf2(0.0), buf3(0.0), cutoffMod(0.0) {
    calculateFeedbackAmount();
  }
  float process(float inputValue);
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
    return fmax(fmin(cutoff + cutoffMod, 0.99), 0.01);
  }
  inline void calculateFeedbackAmount() {
    feedbackAmount = resonance + resonance / (1.0 - getCalculatedCutoff());
  }
};

#endif
