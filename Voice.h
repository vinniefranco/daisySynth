#pragma once

#include "EnvelopeGenerator.h"
#include "Filter.h"
#include <arm_math.h>

#include "daisysp.h"

class Voice {
public:
  friend class VoiceManager;
  uint32_t started_at;
  Voice()
      : mNoteNumber(-1), mVelocity(0), mFilterEnvelopeAmount(0.0), mOscMix(0.5),
        mFilterLFOAmount(0.0), mOscOnePitchAmount(0.0), mOscTwoPitchAmount(0.0),
        mLFOValue(0.0), isActive(false) {
    mVolumeEnvelope.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
    mFilter.Init(48000.0f);
  };

  inline void setFilterEnvelopeAmount(float amount) {
    mFilterEnvelopeAmount = amount;
  }
  inline void setFilterLFOAmount(float amount) { mFilterLFOAmount = amount; }
  inline void setOscOnePitchAmount(float amount) {
    mOscOnePitchAmount = amount;
  }
  inline void setOscTwoPitchAmount(float amount) {
    mOscTwoPitchAmount = amount;
  }
  inline void setOscMix(float amount) { mOscMix = amount; }
  inline void setLFOValue(float value) { mLFOValue = value; }
  inline void setNoteNumber(int noteNumber) {
    mNoteNumber = noteNumber;
    float frequency = daisysp::mtof(noteNumber);
    mOscOne.SetFreq(frequency);
    mOscTwo.SetFreq(frequency);
  }
  float nextSample();
  void setFree();
  void reset();

private:
  daisysp::Oscillator mOscOne;
  daisysp::Oscillator mOscTwo;
  EnvelopeGenerator mVolumeEnvelope;
  EnvelopeGenerator mFilterEnvelope;
  // Filter mFilter;
  daisysp::MoogLadder mFilter;
  int mNoteNumber;
  int mVelocity;
  float mFilterEnvelopeAmount;
  float mOscMix;
  float mFilterLFOAmount;
  float mOscOnePitchAmount;
  float mOscTwoPitchAmount;
  float mLFOValue;
  bool isActive;
};
