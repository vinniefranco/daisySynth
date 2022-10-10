#pragma once

#include "EnvelopeGenerator.h"
#include <arm_math.h>

#include "WaveOsc.h"

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
    noise.Init(48000.0f);
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
  inline void setNoteNumber(int noteNumber, float note) {
    mNoteNumber = noteNumber;
    mOscOne.SetFreq(note);
    mOscTwo.SetFreq(note - 0.7f);
  }
  float nextSample();
  void setFree();
  void reset();

private:
  WaveOsc mOscOne;
  WaveOsc mOscTwo;
  daisysp::MoogLadder mFilter;
  daisysp::Jitter noise;
  EnvelopeGenerator mVolumeEnvelope;
  EnvelopeGenerator mFilterEnvelope;
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
