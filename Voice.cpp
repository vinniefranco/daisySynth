#include "Voice.h"

float Voice::nextSample() {
  if (!isActive)
    return 0.0;
  float oscillatorOneOutput = mOscOne.Process();
  float oscillatorTwoOutput = mOscTwo.Process();
  float oscillatorSum =
      ((1 - mOscMix) * oscillatorOneOutput) + (mOscMix * oscillatorTwoOutput);

  // float volumeEnvelopeValue = mVolumeEnvelope.nextSample();
  // float filterEnvelopeValue = mFilterEnvelope.nextSample();

  // mFilter.setCutoffMod(filterEnvelopeValue * mFilterEnvelopeAmount +
  //                      mLFOValue * mFilterLFOAmount);

  // mOscOne.setPitchMod(mLFOValue * mOscOnePitchAmount);
  // mOscTwo.setPitchMod(mLFOValue * mOscTwoPitchAmount);

  // return mFilter.process(oscillatorSum * volumeEnvelopeValue * mVelocity /
  //                        127.0);

  return oscillatorSum * mVelocity / 127.0;
}
void Voice::setFree() { isActive = false; }
void Voice::reset() {
  mNoteNumber = -1;
  mVelocity = 0;
  mOscOne.Reset(0);
  mOscTwo.Reset(0);
  // mVolumeEnvelope.reset();
  // mFilterEnvelope.reset();
  // mFilter.reset();
}
