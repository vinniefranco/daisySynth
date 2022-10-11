#include "Voice.h"

float Voice::nextSample() {
  if (!isActive)
    return 0.0;

  float oscillatorOneOutput = osc0_.Process() + noise_.Process();
  float oscillatorTwoOutput = osc1_.Process();
  float oscillatorSum = oscillatorOneOutput + oscillatorTwoOutput;

  float volumeEnvelopeValue = v_env.process();

  float filterEnvelopeValue = f_env.process();

  flt.setCutoffMod(filterEnvelopeValue * mFilterEnvelopeAmount +
                   (mLFOValue * mFilterLFOAmount));

  if (v_env.getState() == v_env.env_idle) {
    setFree();
  }

  return flt.Process(oscillatorSum * volumeEnvelopeValue * velocity / 127.0);
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
