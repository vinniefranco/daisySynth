#pragma once

#include "EnvelopeGenerator.h"
#include "daisysp.h"

#include "EnvMoog.h"
#include "WaveOsc.h"

#include <math.h>

class Voice {
private:
  WaveOsc osc0_;
  WaveOsc osc1_;
  EnvMoog flt;
  daisysp::DelayLine<float, 2> del_;
  daisysp::WhiteNoise noise_;
  EnvelopeGenerator v_env;
  EnvelopeGenerator f_env;
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
    v_env.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
  };

  inline void Init(float new_sample_rate, const int8_t waveform,
                   float osc_amp) {
    size_t fb = 2;
    del_.Init();
    del_.SetDelay(fb);

    osc0_.Init(new_sample_rate);
    osc0_.SetWaveform(waveform);
    osc0_.SetAmp(osc_amp);

    osc1_.Init(new_sample_rate);
    osc1_.SetWaveform(waveform);
    osc1_.SetAmp(osc_amp);

    noise_.Init();
    noise_.SetAmp(0.01f);
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
    osc0_.SetFreq(freq - detune);
    osc1_.SetFreq(freq + detune);
  }
  float nextSample();
  void setFree();
  void reset();
};
