#include "Voice.h"

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
  v_env.Gate(true);
  f_env.Gate(true);
}

void Voice::ClearNoteNumber(int midi_note) {
  if (note_number == midi_note) {
    v_env.Gate(false);
    f_env.Gate(false);
  }
}

void Voice::SetFree() { is_active = false; }
void Voice::Reset() {
  note_number = -1;
  velocity = 0;
  v_env.Reset();
  f_env.Reset();
  flt.Reset();
}
