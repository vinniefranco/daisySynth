#include "Voice.h"

float Voice::Process() {
  float osc0_out = osc0_.Process();
  float osc1_out = osc1_.Process();
  float osc_sum = ((1 - m_osc_mix) * osc0_out) + (m_osc_mix * osc1_out);

  float v_env_value = v_env.Process();
  float f_env_value = f_env.Process();

  flt.setCutoffMod(f_env_value * f_env_amount + (lfo_value * f_lfo_amount));

  if (v_env.GetState() == v_env.IDLE) {
    SetFree();
    return 0.0f;
  }

  float output = flt.Process(osc_sum * v_env_value * velocity / 127.0);

  return output;
}

void Voice::Reset() {
  note_number = -1;
  velocity = 0;
  v_env.Reset();
  f_env.Reset();
  flt.reset();
  ResetPhasor();
}

void Voice::SetFree() { is_active = false; }
