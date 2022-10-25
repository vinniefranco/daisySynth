#pragma once

#include "daisysp.h"

#include "ADSR.h"
#include "EnvFilter.h"
#include "WaveTableOsc.h"

class Voice {
private:
  uint32_t age;
  float bend;
  float detune;
  float freq = 0.0f;
  ADSR f_env;
  float f_env_amount;
  float f_lfo_amount;
  ADSR v_env;
  EnvFilter flt;
  WaveTableOsc osc0_;
  WaveTableOsc osc1_;
  bool is_active;
  bool is_stealable;
  float key_follow_amount;
  float lfo_value;
  float mOscOnePitchAmount;
  float mOscTwoPitchAmount;
  float m_osc_mix;
  int note_number;
  float rand_walk[6] = {0.02f, 0.008f, 0.03f, 0.003f, 0.05f, 0.1f};
  float velocity;
  uint8_t walk_cursor;

public:
  friend class VoiceManager;

  Voice()
      : age(0), bend(1.0f), detune(0.01f), freq(0.0f), is_active(false),
        f_env_amount(0.0f), note_number(-1), velocity(0.0f), m_osc_mix(0.5f),
        f_lfo_amount(0.0f), is_stealable(false), mOscOnePitchAmount(1.0f),
        mOscTwoPitchAmount(1.0f), lfo_value(0.0f), walk_cursor(0){};

  void Init(float new_sample_rate, float osc_amp);

  float Process();

  void ClearNoteNumber(int midi_note);
  void IncrementAge();
  void Reset();
  void ResetPhasor();
  void SetDetune(float new_detune);
  void SetFilterEnvelopeAmount(float amount);
  void SetFilterLFOAmount(float amount);
  void SetFree();
  void SetLFOValue(float value);
  void SetNoteNumber(int midi_note, float new_freq, float new_velocity,
                     float key_follow_amount);
  void SetOscMix(float amount);
  void SetOscOnePitchAmount(float amount);
  void SetOscTwoPitchAmount(float amount);
  void SetPitchBend(float amount);
  void SetWavetable(WaveSlot *wt_slots);
};
