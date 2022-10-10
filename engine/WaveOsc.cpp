#include "WaveOsc.h"

void WaveOsc::SetFreq(float freq) {
  table_delta = freq * table_size_over_sample_rate;
}

float WaveOsc::Process() {
  auto i0 = (unsigned int)current_index;
  auto i1 = i0 + 1;
  auto frac = current_index - (float)i0;
  auto v0 = wt[i0];
  auto v1 = wt[i1];

  float current_sample = v0 + frac * (v1 - v0);
  if ((current_index += table_delta) > 512.f)
    current_index -= 512.f;

  return current_sample * amp;
}
