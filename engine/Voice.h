#pragma once

#include "daisysp.h"

#include "ADSR.h"
#include "EnvFilter.h"
#include "WaveTableOsc.h"

class Voice {
private:
  ADSR f_env;
  ADSR v_env;
  EnvFilter flt;
  WaveTableOsc osc0_;
  WaveTableOsc osc1_;
  bool is_active;
  float bend = 1.0f;
  float detune = 0.1f;
  float mOscOnePitchAmount;
  float mOscTwoPitchAmount;
  float f_env_amount;
  float f_lfo_amount;
  float freq = 0.0f;
  float lfo_value;
  float m_osc_mix;
  int note_number;
  int velocity;

public:
  friend class VoiceManager;
  int panning = 3;
  uint32_t started_at;
  Voice()
      : is_active(false), f_env_amount(0.0f), note_number(-1), velocity(0),
        m_osc_mix(0.5f), f_lfo_amount(0.0f), mOscOnePitchAmount(1.0f),
        mOscTwoPitchAmount(1.0f), lfo_value(0.0f){};

  inline void Init(float new_sample_rate, float osc_amp) {
    osc0_.Init(new_sample_rate);
    osc1_.Init(new_sample_rate);

    v_env.setAttackRate(.1f * new_sample_rate);
    v_env.setDecayRate(.3f * new_sample_rate);
    v_env.setSustainLevel(.7f);
    v_env.setReleaseRate(2.7f * new_sample_rate);

    f_env.setAttackRate(.1f * new_sample_rate);
    f_env.setDecayRate(.3f * new_sample_rate);
    f_env.setSustainLevel(.7f);
    f_env.setReleaseRate(2.7f * new_sample_rate);
  }

  inline void setFilterEnvelopeAmount(float amount) { f_env_amount = amount; }
  inline void setFilterLFOAmount(float amount) { f_lfo_amount = amount; }
  inline void setOscOnePitchAmount(float amount) {
    mOscOnePitchAmount = amount;
  }
  inline void setOscTwoPitchAmount(float amount) {
    mOscTwoPitchAmount = amount;
  }

  inline void SetPitchBend(float amount) {
    bend = amount;
    osc0_.SetFreq((freq - detune * mOscOnePitchAmount) * bend);
    osc1_.SetFreq((freq + detune) * bend);
  }
  inline void setOscMix(float amount) { m_osc_mix = amount; }
  inline void setDetune(float new_detune) { detune = new_detune; }
  inline void setLFOValue(float value) { lfo_value = value; }
  inline void setNoteNumber(int midi_note, float new_freq) {
    note_number = midi_note;
    freq = new_freq;
    osc0_.SetFreq((freq * mOscOnePitchAmount) * bend);
    osc1_.SetFreq((freq * mOscTwoPitchAmount + detune) * bend);
  }
  inline void SetWavetable(waveTable *wt, int total_slots) {
    osc0_.SetWavetable(wt, total_slots);
    osc1_.SetWavetable(wt, total_slots);
  }
  float nextSample();
  void setFree();
  void reset();
};
