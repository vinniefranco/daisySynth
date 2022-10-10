#pragma once

#include "EnvelopeGenerator.h"
#include "daisysp.h"

#include "WaveOsc.h"

#include <math.h>

class Voice {
private:
  WaveOsc osc0;
  WaveOsc osc1;
  daisysp::MoogLadder flt;
  daisysp::WhiteNoise noise;
  EnvelopeGenerator mVolumeEnvelope;
  EnvelopeGenerator mFilterEnvelope;
  int note_number;
  int mVelocity;
  float detune = 0.0f;
  float mFilterEnvelopeAmount;
  float mOscMix;
  float mFilterLFOAmount;
  float mOscOnePitchAmount;
  float mOscTwoPitchAmount;
  float mLFOValue;
  bool isActive;

public:
  friend class VoiceManager;
  uint32_t started_at;
  Voice()
      : note_number(-1), mVelocity(0), mFilterEnvelopeAmount(0.0), mOscMix(0.5),
        mFilterLFOAmount(0.0), mOscOnePitchAmount(0.0), mOscTwoPitchAmount(0.0),
        mLFOValue(0.0), isActive(false) {
    mVolumeEnvelope.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
  };

  inline void Init(float new_sample_rate, const int8_t waveform,
                   float osc_amp) {
    osc0.Init(new_sample_rate);
    osc0.SetWaveform(waveform);
    osc0.SetAmp(osc_amp);

    osc1.Init(new_sample_rate);
    osc1.SetWaveform(waveform);
    osc1.SetAmp(osc_amp);

    flt.Init(new_sample_rate);
    noise.Init();
    noise.SetAmp(0.01f);
  }

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
  inline void setDetune(float new_detune) { detune = new_detune; }
  inline void setLFOValue(float value) { mLFOValue = value; }
  inline void setNoteNumber(int midi_note, float freq) {
    note_number = midi_note;
    osc0.SetFreq(freq - detune);
    osc1.SetFreq(freq + detune);
  }
  float nextSample();
  void setFree();
  void reset();
};
