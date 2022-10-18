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
      : is_active(false), mOscOnePitchAmount(1.0f), mOscTwoPitchAmount(1.0f),
        f_env_amount(0.0f), m_osc_mix(0.5f), velocity(0), f_lfo_amount(0.0f),
        lfo_value(0.0f), note_number(-1), age(0){};

  inline void Init(float new_sample_rate, float osc_amp) {
    osc0_.Init(new_sample_rate);
    osc1_.Init(new_sample_rate);

    v_env.SetAttackRate(.1f * new_sample_rate);
    v_env.SetDecayRate(.3f * new_sample_rate);
    v_env.SetSustainLevel(.7f);
    v_env.SetReleaseRate(2.7f * new_sample_rate);

    f_env.SetAttackRate(.1f * new_sample_rate);
    f_env.SetDecayRate(.3f * new_sample_rate);
    f_env.SetSustainLevel(.7f);
    f_env.SetReleaseRate(2.7f * new_sample_rate);
  }

  inline void SetFilterEnvelopeAmount(float amount) { f_env_amount = amount; }
  inline void SetFilterLFOAmount(float amount) { f_lfo_amount = amount; }
  inline void ResetPhasor() {
    osc0_.ResetPhasor();
    osc1_.ResetPhasor();
  }
  inline void SetOscOnePitchAmount(float amount) {
    mOscOnePitchAmount = amount;
  }
  inline void SetOscTwoPitchAmount(float amount) {
    mOscTwoPitchAmount = amount;
  }

  inline void SetPitchBend(float amount) {
    bend = amount;
    osc0_.SetFreq((freq - detune * mOscOnePitchAmount) * bend);
    osc1_.SetFreq((freq + detune) * bend);
  }
  inline void SetOscMix(float amount) { m_osc_mix = amount; }
  inline void SetDetune(float new_detune) { detune = new_detune; }
  inline void SetLFOValue(float value) { lfo_value = value; }
  inline void ClearNoteNumber(int midi_note) {
    if (note_number == midi_note) {
      v_env.Gate(false);
      f_env.Gate(false);
    }
  }
  inline void SetNoteNumber(int midi_note, float new_freq, int new_velocity) {
    if (note_number != midi_note) {
      Reset();
      ResetPhasor();
    }

    note_number = midi_note;
    freq = new_freq;

    osc0_.SetFreq((freq * mOscOnePitchAmount) * bend);
    osc1_.SetFreq((freq * mOscTwoPitchAmount + detune) * bend);

    velocity = new_velocity;
    is_active = true;
    v_env.Gate(true);
    f_env.Gate(true);
  }
  inline void SetWavetable(waveTable *saw, int saw_slots, waveTable *sqr,
                           int sqr_slots) {
    osc0_.SetWavetable(saw, saw_slots);
    osc1_.SetWavetable(sqr, sqr_slots);
  }
  float Process();
  void SetFree();
  void Reset();
};
