#include "Voice.h"

float Voice::nextSample() {
  if (!isActive)
    return 0.0f;

  float oscillatorOneOutput = osc0_->Process();
  float oscillatorTwoOutput = osc1_.Process();
  float oscillatorSum = oscillatorOneOutput + oscillatorTwoOutput;
  oscillatorSum = oscillatorOneOutput;

  float volumeEnvelopeValue = v_env.process();
  float filterEnvelopeValue = f_env.process();

  flt.setCutoffMod(filterEnvelopeValue * mFilterEnvelopeAmount +
                   (mLFOValue * mFilterLFOAmount));

  if (v_env.getState() == v_env.env_idle) {
    setFree();
    return 0.0f;
  }

  float output =
      flt.Process(oscillatorSum * volumeEnvelopeValue * velocity / 127.0);
  // Quick hard clipping
  if (output > 1.f)
    output = 1.f;

  if (output < -1.f)
    output = -1.f;

  return output;
}
void Voice::setFree() { isActive = false; }
void Voice::reset() {
  note_number = -1;
  velocity = 0;
  // osc0_.Reset();
  osc1_.Reset();
  v_env.reset();
  f_env.reset();
  flt.reset();
}
