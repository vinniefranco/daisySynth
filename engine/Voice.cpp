#include "Voice.h"

void Voice::Init(float new_sample_rate, float osc_amp) {
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

void Voice::ClearNoteNumber(int midi_note) {
  if (note_number == midi_note) {
    is_stealable = true;
    v_env.Gate(false);
    f_env.Gate(false);
  }
}

void Voice::IncrementAge() { age++; }

float Voice::Process() {
  float osc0_out = osc0_.Process();
  float osc1_out = osc1_.Process();
  float osc_sum = ((1 - m_osc_mix) * osc0_out) + (m_osc_mix * osc1_out);

  float v_env_value = v_env.Process();
  float f_env_value = f_env.Process();

  flt.SetCutoffMod(f_env_value * (f_env_amount + key_follow_amount) +
                   (lfo_value * f_lfo_amount));

  if (v_env.GetState() == v_env.ENV_IDLE) {
    SetFree();
    return 0.0f;
  }

  float output = flt.Process(osc_sum) * v_env_value * velocity;

  return output;
}

void Voice::ResetPhasor() {
  osc0_.ResetPhasor();
  osc1_.ResetPhasor();
}

void Voice::SetNoteNumber(int midi_note, float new_freq, float new_velocity,
                          float new_key_follow_amount) {
  if (note_number != midi_note) {
    Reset();
    ResetPhasor();
  }

  key_follow_amount = new_key_follow_amount;
  note_number = midi_note;
  freq = new_freq + rand_walk[walk_cursor % 6];

  walk_cursor++;

  osc0_.SetFreq((freq * mOscOnePitchAmount) * bend);
  osc1_.SetFreq((freq * mOscTwoPitchAmount + detune) * bend);

  velocity = new_velocity;
  is_active = true;
  is_stealable = false;
  v_env.Gate(true);
  f_env.Gate(true);
}

void Voice::Reset() {
  note_number = -1;
  velocity = 0;
  v_env.Reset();
  f_env.Reset();
  flt.Reset();
}

void Voice::SetDetune(float new_detune) { detune = new_detune; }
void Voice::SetFilterEnvelopeAmount(float amount) { f_env_amount = amount; }
void Voice::SetFilterLFOAmount(float amount) { f_lfo_amount = amount; }

void Voice::SetFree() { is_active = false; }

void Voice::SetLFOValue(float value) { lfo_value = value; }

void Voice::SetOscMix(float amount) { m_osc_mix = amount; }
void Voice::SetOscOnePitchAmount(float amount) { mOscOnePitchAmount = amount; }
void Voice::SetOscTwoPitchAmount(float amount) { mOscTwoPitchAmount = amount; }

void Voice::SetPitchBend(float amount) {
  bend = amount;
  osc0_.SetFreq((freq - detune * mOscOnePitchAmount) * bend);
  osc1_.SetFreq((freq + detune) * bend);
}

void Voice::SetWavetable(WaveSlot *wt_slots) {
  osc0_.SetWavetable(wt_slots[0].wt, wt_slots[0].wt_slots);
  osc1_.SetWavetable(wt_slots[0].wt, wt_slots[0].wt_slots);
}
