#include "Voice.h"

float Voice::nextSample() {
  if (!isActive)
    return 0.0;

  float oscillatorOneOutput = osc0_.Process() + noise_.Process();
  float oscillatorTwoOutput = osc1_.Process();
  float oscillatorSum = oscillatorOneOutput + oscillatorTwoOutput;

  float volumeEnvelopeValue = v_env.nextSample();

  float filterEnvelopeValue = f_env.nextSample();

  flt.setCutoffMod(filterEnvelopeValue * mFilterEnvelopeAmount +
                   (mLFOValue * mFilterLFOAmount));

  return flt.Process(oscillatorSum * velocity / 127.0) * volumeEnvelopeValue;
}
void Voice::setFree() { isActive = false; }
void Voice::reset() {
  note_number = -1;
  velocity = 0;
  osc0_.Reset();
  osc1_.Reset();
  v_env.reset();
  f_env.reset();
  flt.reset();
}
