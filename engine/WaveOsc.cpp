#include "WaveOsc.h"

void WaveOsc::SetFreq(float freq) {
  table_delta_ = freq * table_size_over_sample_rate_;
}

float WaveOsc::Process() {
  // Hermite interpolation
  auto i0 = (unsigned int)current_index_;
  auto i1 = i0 + 1;
  auto i2 = i0 + 2;
  auto i3 = i0 + 3;
  auto v0 = wt_[i0];
  auto v1 = wt_[i1 % WT_SAMPS];
  auto v2 = wt_[i2 % WT_SAMPS];
  auto v3 = wt_[i3 % WT_SAMPS];
  auto frac = current_index_ - (float)i0;
  float slope0 = (v2 - v0) * .5f;
  float slope1 = (v3 - v1) * .5f;
  float v = v1 - v2;
  float w = slope0 + v;
  float a = w + v + slope1;
  float b_neg = w + a;
  float stage1 = a * frac - b_neg;
  float stage2 = stage1 * frac + slope0;

  float wt_samps = (float)WT_SAMPS;
  if ((current_index_ += table_delta_) > wt_samps)
    current_index_ -= wt_samps;

  return stage2 * frac + v1;
}
