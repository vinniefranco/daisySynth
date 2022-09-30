// By Paul Kellett
// http://www.musicdsp.org/showone.php?id=29
#include "EnvFilter.h"

/**
calculates a lowpass, highpass and bandpass filter dependng on the mode selected
*/
float EnvFilter::Process(float inputValue) {
  if (inputValue == 0.0)
    return inputValue;
  float calculatedCutoff = GetCalculatedCutoff();
  //
  buf0 +=
      calculatedCutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
  buf1 += calculatedCutoff * (buf0 - buf1);
  buf2 += calculatedCutoff * (buf1 - buf2);
  buf3 += calculatedCutoff * (buf2 - buf3);
  switch (mode) {
  case FILTER_MODE_LOWPASS:
    return buf3;
  case FILTER_MODE_HIGHPASS:
    return inputValue - buf3;
  case FILTER_MODE_BANDPASS:
    return buf0 - buf3;
  default:
    return 0.0;
  }
}

void EnvFilter::SetCutoff(float newCutoff) {
  cutoff = newCutoff;
  CalculateFeedbackAmount();
};

void EnvFilter::SetResonance(float newResonance) {
  resonance = newResonance;
  CalculateFeedbackAmount();
};

void EnvFilter::SetFilterMode(FilterMode newMode) { mode = newMode; }

void EnvFilter::SetCutoffMod(float newCutoffMod) {
  cutoffMod = newCutoffMod;
  CalculateFeedbackAmount();
}

void EnvFilter::Reset() { buf0 = buf1 = buf2 = buf3 = 0.0f; }

float EnvFilter::GetCalculatedCutoff() const {
  return daisysp::fmax(daisysp::fmin(cutoff + cutoffMod, 0.99), 0.01);
}

void EnvFilter::CalculateFeedbackAmount() {
  feedbackAmount = resonance + resonance / (1.0 - GetCalculatedCutoff());
}
