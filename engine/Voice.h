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
  int velocity;

public:
  friend class VoiceManager;
  int note_number;
  int panning = 3;
  uint32_t age;
  Voice()
      : is_active(false), f_env_amount(0.0f), note_number(-1), velocity(0),
        m_osc_mix(0.5f), f_lfo_amount(0.0f), mOscOnePitchAmount(1.0f),
        mOscTwoPitchAmount(1.0f), lfo_value(0.0f), age(0){};

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
  inline void ResetPhasor() {
    osc0_.ResetPhasor();
    osc1_.ResetPhasor();
  }
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
  inline void clearNoteNumber(int midi_note) {
    if (note_number == midi_note) {
      v_env.gate(false);
      f_env.gate(false);
    }
  }
  inline void setNoteNumber(int midi_note, float new_freq, int new_velocity) {
    if (note_number != midi_note) {
      reset();
      ResetPhasor();
    }

    note_number = midi_note;
    freq = new_freq;

    osc0_.SetFreq((freq * mOscOnePitchAmount) * bend);
    osc1_.SetFreq((freq * mOscTwoPitchAmount + detune) * bend);

    velocity = new_velocity;
    is_active = true;
    v_env.gate(true);
    f_env.gate(true);
  }
  inline void SetWavetable(WaveSlot *wt_slots) {
    osc0_.SetWavetable(wt_slots[0].wt, wt_slots[0].wt_slots);
    osc1_.SetWavetable(wt_slots[0].wt, wt_slots[0].wt_slots);
  }
  float nextSample();
  void setFree();
  void reset();
};
