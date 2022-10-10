#include "Utility/dsp.h"
#include "daisy_seed.h"

constexpr uint16_t WT_SIZE = 2048;

class WaveOsc {
public:
  enum { WAVE_SAW, WAVE_SIN, WAVE_SQUARE };

  WaveOsc(){};
  ~WaveOsc(){};

  void Init(float new_sample_rate) {
    sample_rate_ = new_sample_rate;
    waveform_ = WAVE_SIN;
  }

  float Process();

  inline void Reset() {
    current_index_ = 0.0f;
    table_delta_ = 0.0f;
  }

  inline void SetAmp(float new_amp) { amp_ = new_amp; }

  void SetFreq(float freq);

  inline void SetWaveform(const uint8_t wf) {
    table_size_over_sample_rate_ = (float)(WT_SIZE - 1) / sample_rate_;

    switch (wf) {
    case WAVE_SAW: {
      for (uint16_t i = 0; i < WT_SIZE; ++i) {
        wt_[i] = (float)(i / (float)WT_SIZE * 2.f) - 1.0f;
      }
      break;
    }
    case WAVE_SIN: {
      auto angle_delta = TWOPI_F / (float)(WT_SIZE - 1);
      auto current_angle = 0.0f;

      for (uint16_t i = 0; i < WT_SIZE; ++i) {
        auto sample = std::sin(current_angle);
        wt_[i] = sample;
        current_angle += angle_delta;
      }
      break;
    }
    case WAVE_SQUARE: {
      for (uint16_t i = 0; i < WT_SIZE; ++i) {
        wt_[i] = (i < WT_SIZE / 2) ? -1.f : 1.f;
      }
      break;
    }
    }
  }

private:
  uint8_t waveform_;

  // This might need to move to a wt slots concept in SDRAM.
  float wt_[WT_SIZE];

  float amp_;
  float current_index_ = 0.0f;
  float sample_rate_;
  float table_delta_ = 0.0f;
  float table_size_over_sample_rate_;
};
