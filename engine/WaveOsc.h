#include "Utility/dsp.h"
#include "arm_math.h"

#define WT_SAMPS 2048

class WaveOsc {
public:
  WaveOsc(){};
  ~WaveOsc(){};

  enum { WAVE_SIN, WAVE_SAW, WAVE_SQUARE };

  void Init(float new_sample_rate) {
    sample_rate_ = new_sample_rate;
    waveform_ = WAVE_SIN;
  }

  inline void SetAmp(float new_amp) { amp_ = new_amp; }

  inline void SetWaveform(const uint8_t wf) {
    table_size_over_sample_rate_ = (float)(WT_SAMPS - 1) / sample_rate_;

    switch (wf) {
    case WAVE_SAW:
      for (uint16_t i = 0; i < WT_SAMPS; ++i) {
        wt_[i] = (float)(i / (float)WT_SAMPS * 2.f) - 1.0f;
      }
      break;
    case WAVE_SQUARE:
      for (uint16_t i = 0; i < WT_SAMPS; ++i) {
        wt_[i] = (i < WT_SAMPS / 2) ? -1.f : 1.f;
      }
      break;
    case WAVE_SIN:
      auto angle_delta = TWOPI_F / (float)(WT_SAMPS - 1);
      auto current_angle = 0.0f;

      for (uint16_t i = 0; i < WT_SAMPS; ++i) {
        auto sample = std::sin(current_angle);
        wt_[i] = sample;
        current_angle += angle_delta;
      }
      break;
    }
  }

  float Process();

  inline void Reset() {
    current_index_ = 0.0f;
    table_delta_ = 0.0f;
  }

  void SetFreq(float freq);

private:
  uint8_t waveform_;
  float amp_;
  float current_index_ = 0.0f;
  float sample_rate_;
  float table_delta_ = 0.0f;
  float table_size_over_sample_rate_;
  float wt_[WT_SAMPS];
};
