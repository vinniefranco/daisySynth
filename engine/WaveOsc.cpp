#include "WaveOsc.h"

void WaveOsc::SetFreq(float freq) {
  table_delta_ = freq * table_size_over_sample_rate_;
}

float WaveOsc::Process() {
  auto i0 = (unsigned int)current_index_;
  auto i1 = i0 + 1;
  auto frac = current_index_ - (float)i0;
  auto v0 = wt_[i0];
  auto v1 = wt_[i1];

  float current_sample = v0 + frac * (v1 - v0);
  float wt_samps = (float)WT_SAMPS;
  if ((current_index_ += table_delta_) > wt_samps)
    current_index_ -= wt_samps;

  return current_sample * amp_;
}
