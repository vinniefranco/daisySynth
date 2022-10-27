#include "Voice.h"

void Voice::Init(float new_sample_rate, float osc_amp) {
  sample_rate = new_sample_rate;
  half_second = sample_rate / 2;

  osc0_.Init(new_sample_rate);
  osc1_.Init(new_sample_rate);
  osc2_.Init(new_sample_rate);

  v_env.Init(new_sample_rate);
  v_env.SetAttackRate(.1f);
  v_env.SetDecayRate(.3f);
  v_env.SetSustainLevel(.7f);
  v_env.SetReleaseRate(2.7f);
  v_env.SetReleaseRate(2.7f);
  v_env.SetKillRate(.03f);

  f_env.Init(new_sample_rate);
  f_env.SetAttackRate(.1f);
  f_env.SetDecayRate(.3f);
  f_env.SetSustainLevel(.7f);
  f_env.SetReleaseRate(2.7f);
  f_env.SetKillRate(.03f);
}

void Voice::ClearNoteNumber(int midi_note) {
  if (note.midi == midi_note) {
    state = VOICE_STEALABLE;

    v_env.Gate(false);
    f_env.Gate(false);
  }
}

void Voice::IncrementAge() { age++; }

float Voice::Process() {
  if (v_env.GetState() == v_env.ENV_IDLE) {
    if (state == VOICE_STOLEN) {
      SetNote(next_note);
      return 0.0f;
    } else {
      SetFree();
      return 0.0f;
    }
  }

  float osc0_out = osc0_.Process();
  float osc1_out = osc1_.Process();
  float osc2_out = osc2_.Process();
  float osc_sum =
      ((1 - m_osc_mix) * osc0_out) + (m_osc_mix * osc1_out) + (0.3f * osc2_out);

  float v_env_value = v_env.Process();
  float f_env_value = f_env.Process();

  flt.SetCutoffMod(f_env_value * (f_env_amount + note.key_follow) +
                   (lfo_value * f_lfo_amount));

  float output = flt.Process(osc_sum) * v_env_value * note.velocity;

  // // Scavenge when they've been quiet for half a second
  // // FIXME:: This feels ghetto.
  // if (output == 0.0f) {
  //   cursor++;

  //   if (cursor > half_second) {
  //     cursor = 0;
  //     Reset();
  //     ResetPhasor();
  //     SetFree();

  //     return 0.0f;
  //   }
  // }

  return output;
}

void Voice::ResetPhasor() {
  osc0_.ResetPhasor();
  osc1_.ResetPhasor();
  osc2_.ResetPhasor();
}

void Voice::StealVoice(Note new_note) {
  next_note = new_note;
  state = VOICE_STOLEN;
  v_env.Kill();
  f_env.Kill();
}

bool Voice::IsPlayable() {
  return (state == VOICE_STEALABLE || state == VOICE_PLAYING ||
          state == VOICE_STOLEN);
}

void Voice::SetNote(Note new_note) {
  if (note.midi != new_note.midi) {
    Reset();
    ResetPhasor();
  }

  note = new_note;
  note.freq = note.freq + rand_walk[walk_cursor % 6];

  walk_cursor++;

  osc0_.SetFreq((note.freq * mOscOnePitchAmount) * bend);
  osc1_.SetFreq((note.freq * mOscTwoPitchAmount + detune) * bend);
  osc2_.SetFreq((note.freq * mOscTwoPitchAmount + (detune * 2)) * bend);

  state = VOICE_PLAYING;
  v_env.Gate(true);
  f_env.Gate(true);
}

void Voice::Reset() {
  v_env.Reset();
  f_env.Reset();
  flt.Reset();
}

void Voice::SetDetune(float new_detune) { detune = new_detune; }
void Voice::SetFilterEnvelopeAmount(float amount) { f_env_amount = amount; }
void Voice::SetFilterLFOAmount(float amount) { f_lfo_amount = amount; }

void Voice::SetFree() { state = VOICE_FREE; }

void Voice::SetLFOValue(float value) { lfo_value = value; }

void Voice::SetOscMix(float amount) { m_osc_mix = amount; }
void Voice::SetOscOnePitchAmount(float amount) { mOscOnePitchAmount = amount; }
void Voice::SetOscTwoPitchAmount(float amount) { mOscTwoPitchAmount = amount; }

void Voice::SetPitchBend(float amount) {
  bend = amount;
  osc0_.SetFreq((note.freq - detune * mOscOnePitchAmount) * bend);
  osc1_.SetFreq((note.freq + detune) * bend);
}

void Voice::SetWavetable(WaveSlot *wt_slots) {
  osc0_.SetWavetable(wt_slots[0].wt, wt_slots[0].wt_slots);
  osc1_.SetWavetable(wt_slots[0].wt, wt_slots[0].wt_slots);
  osc2_.SetWavetable(wt_slots[0].wt, wt_slots[0].wt_slots);
}
