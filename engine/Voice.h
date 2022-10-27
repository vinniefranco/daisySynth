#pragma once

#include "daisysp.h"

#include "ADSR.h"
#include "EnvFilter.h"
#include "Note.h"
#include "WaveTableOsc.h"
#include <math.h>

class Voice {
private:
  int32_t age;
  float bend;
  float detune;
  ADSR f_env;
  float f_env_amount;
  float f_lfo_amount;
  ADSR v_env;
  EnvFilter flt;
  WaveTableOsc osc0_;
  WaveTableOsc osc1_;
  WaveTableOsc osc2_;
  float lfo_value;
  float mOscOnePitchAmount;
  float mOscTwoPitchAmount;
  float m_osc_mix;
  Note note;
  Note next_note;
  float rand_walk[6] = {0.02f, 0.008f, 0.03f, 0.003f, 0.05f, 0.1f};
  float sample_rate;
  int state;
  uint8_t walk_cursor;
  uint32_t half_second;
  uint32_t cursor;

public:
  friend class VoiceManager;

  Voice()
      : age(0), bend(1.0f), detune(0.01f), f_env_amount(0.0f),
        f_lfo_amount(0.0f), lfo_value(0.0f), mOscOnePitchAmount(1.0f),
        mOscTwoPitchAmount(1.0f), m_osc_mix(0.5f), state(VOICE_FREE),
        walk_cursor(0), cursor(0){};

  enum voiceState {
    VOICE_FREE = 0,
    VOICE_PLAYING,
    VOICE_STEALABLE,
    VOICE_STOLEN
  };

  void Init(float new_sample_rate, float osc_amp);

  float Process();

  bool IsPlayable();

  void StealVoice(Note new_note);
  void ClearNoteNumber(int midi_note);
  void IncrementAge();
  void Reset();
  void ResetPhasor();
  void SetDetune(float new_detune);
  void SetFilterEnvelopeAmount(float amount);
  void SetFilterLFOAmount(float amount);
  void SetFree();
  void SetLFOValue(float value);
  void SetNote(Note new_note);
  void SetOscMix(float amount);
  void SetOscOnePitchAmount(float amount);
  void SetOscTwoPitchAmount(float amount);
  void SetPitchBend(float amount);
  void SetWavetable(WaveSlot *wt_slots);
};
