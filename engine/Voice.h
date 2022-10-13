#pragma once

#include "daisy_seed.h"
#include "daisysp.h"

#include "ADSR.h"
#include "EnvFilter.h"
#include "WaveOsc.h"
#include "WaveUtils.h"

#include <math.h>

class Voice {
private:
  // WaveOsc osc0_;
  WaveTableOsc *osc0_;
  WaveOsc osc1_;
  // daisysp::Oscillator osc0_;
  // daisysp::Oscillator osc1_;
  EnvFilter flt;
  daisysp::WhiteNoise noise_;
  ADSR v_env;
  ADSR f_env;
  int note_number;
  int velocity;
  float detune = 0.1f;
  float mFilterEnvelopeAmount;
  float mOscMix;
  float mFilterLFOAmount;
  float mOscOnePitchAmount;
  float mOscTwoPitchAmount;
  float mLFOValue;
  bool isActive;

public:
  friend class VoiceManager;
  int panning = 3;
  uint32_t started_at;
  Voice()
      : note_number(-1), velocity(0), mFilterEnvelopeAmount(0.0f),
        mOscMix(0.5f), mFilterLFOAmount(0.0f), mOscOnePitchAmount(0.0f),
        mOscTwoPitchAmount(0.0f), mLFOValue(0.0f), isActive(false){};

  inline void Init(float new_sample_rate, const int8_t waveform,
                   float osc_amp) {
    osc0_ = sawOsc();
    // osc0_.Init(new_sample_rate);
    // osc0_.SetWaveform(waveform);
    // osc0_.SetAmp(osc_amp);

    osc1_.Init(new_sample_rate);
    osc1_.SetWaveform(waveform);
    osc1_.SetAmp(osc_amp);

    v_env.setAttackRate(.1f * new_sample_rate);
    v_env.setDecayRate(.3f * new_sample_rate);
    v_env.setSustainLevel(.7f);
    v_env.setReleaseRate(2.7f * new_sample_rate);

    f_env.setAttackRate(.1f * new_sample_rate);
    f_env.setDecayRate(.3f * new_sample_rate);
    f_env.setSustainLevel(.7f);
    f_env.setReleaseRate(2.7f * new_sample_rate);

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
    // osc0_.SetFreq(freq - detune);
    osc0_->SetFreq(freq - detune);
    osc1_.SetFreq(freq + detune);
  }
  float nextSample();
  void setFree();
  void reset();
};
