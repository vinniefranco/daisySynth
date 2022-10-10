#include "Voice.h"

float Voice::nextSample() {
  if (!isActive)
    return 0.0;
  float oscillatorOneOutput = osc0.Process() + noise.Process();
  float oscillatorTwoOutput = osc1.Process();
  float oscillatorSum = oscillatorOneOutput + oscillatorTwoOutput;

  float volumeEnvelopeValue = mVolumeEnvelope.nextSample();

  return flt.Process(oscillatorSum * volumeEnvelopeValue * mVelocity / 127.0);
}
void Voice::setFree() { isActive = false; }
void Voice::reset() {
  note_number = -1;
  mVelocity = 0;
  osc0.Reset();
  osc1.Reset();
  mVolumeEnvelope.reset();
  mFilterEnvelope.reset();
  flt.Init(48000.0f);
}
