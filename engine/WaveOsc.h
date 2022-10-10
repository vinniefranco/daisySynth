#include "Utility/dsp.h"
#include "arm_math.h"

class WaveOsc {
public:
  WaveOsc(){};
  ~WaveOsc(){};

  enum { WAVE_SIN, WAVE_SAW };

  void Init(float new_sample_rate) {
    sample_rate = new_sample_rate;
    waveform_ = WAVE_SIN;
  }

  inline void SetAmp(float new_amp) { amp = new_amp; }

  inline void SetWaveform(const uint8_t wf) {
    table_size_over_sample_rate = (float)511 / sample_rate;
    switch (wf) {
    case WAVE_SAW:
      for (uint16_t i = 0; i < 512; ++i) {
        wt[i] = (float)(i / 512.f * 2.f) - 1.0f;
      }
      break;

    case WAVE_SIN:
      sample_rate = sample_rate;
      auto angle_delta = TWOPI_F / (float)(512 - 1);
      auto current_angle = 0.0f;

      for (uint16_t i = 0; i < 512; ++i) {
        auto sample = std::sin(current_angle);
        wt[i] = sample;
        current_angle += angle_delta;
      }
      break;
    }
  }

  float Process();

  inline void Reset() {
    current_index = 0.0f;
    table_delta = 0.0f;
  }

  void SetFreq(float freq);

private:
  uint8_t waveform_;
  float amp;
  float current_index = 0.0f;
  float sample_rate;
  float table_delta = 0.0f;
  float table_size_over_sample_rate;
  float wt[512];
};
