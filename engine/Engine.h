#pragma once

#include "VoiceManager.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "daisy_seed.h"
#include "daisysp.h"
#include "dev/oled_ssd130x.h"
#include "per/gpio.h"

using SynthOledDisplay = daisy::OledDisplay<daisy::SSD130x4WireSpi128x64Driver>;

class Engine {
private:
  // Scope tooling
  uint8_t data_cursor = 0;
  float screen_buffer[128];
  float data_buffer[128];

  float last_cutoff_read = 0.0f;
  float last_res_read = 0.0f;
  float x = 0.8f;
  float cutoff = 0.9f;
  float res = 0.0f;
  uint16_t ticker = 0;
  daisy::DaisySeed *hw;
  SynthOledDisplay::Config disp_cfg;
  SynthOledDisplay display;
  daisy::CpuLoadMeter *load_meter;

  char pot[128];
  char strbuff2[128];

  float midiToFloat(int midi_value) { return (float)midi_value / 127.f; }

public:
  VoiceManager voice_manager;

  Engine() {}
  ~Engine() {}

  void Process(float *left, float *right);
  void writeToUIBuffer(float sample);

  void Init(daisy::DaisySeed *seed, daisy::CpuLoadMeter *meter,
            float sample_rate) {
    hw = seed;
    load_meter = meter;
    // Config Display
    disp_cfg.driver_config.transport_config.pin_config.dc = hw->GetPin(11);
    disp_cfg.driver_config.transport_config.pin_config.reset = hw->GetPin(13);
    display.Init(disp_cfg);

    voice_manager.SetSampleRate(sample_rate);
    voice_manager.SetLFOFrequency(0.03f);

    daisy::System::Delay(1000);
  }

  inline void SetCutoff(float value) {
    if (value > 0.01f) {
      cutoff = 0.9f * (value - cutoff) + cutoff;
    } else {
      cutoff = 0.0f;
    }
  }

  inline void SetRes(int midi_value) { res = midiToFloat(midi_value) - 0.1f; }

  inline float GetCutoff() { return cutoff; }

  inline void SetVol(float new_x) { x = new_x; }
  inline float GetVol() { return x; }

  inline float GetRes() { return res; }

  void tick() {

    if (ticker % 100 == 0) {
      const float avg_load = load_meter->GetAvgCpuLoad();

      display.Fill(false);

      sprintf(strbuff2, "LOAD:" FLT_FMT3, FLT_VAR3(avg_load * 100.0f));
      display.SetCursor(0, 0);
      display.WriteString(strbuff2, Font_6x8, true);

      sprintf(pot, "x:" FLT_FMT3, FLT_VAR3(x));
      display.SetCursor(0, 52);
      display.WriteString(pot, Font_6x8, true);

      float neg[128];
      float pos[128];
      for (size_t i = 0; i < 128; i++) {
        pos[i] = 0.0f;
        neg[i] = 0.0f;
        if (screen_buffer[i] >= 0.0000f) {
          pos[i] = screen_buffer[i];
        } else {
          neg[i] = abs(screen_buffer[i]);
        }
      }

      for (size_t i = 0; i < 128; i++) {
        uint8_t y1 = (uint8_t)daisysp::fmap(pos[i] * 1.3f, 1.0f, 32.f,
                                            daisysp::Mapping::EXP);

        uint8_t y2 = (uint8_t)daisysp::fmap(neg[i] * 1.3f, 1.0f, 32.f,
                                            daisysp::Mapping::EXP);

        if (y1 > 0) {
          display.DrawPixel(i, 10 + y1, true);
        }
        if (y2 > 0) {
          display.DrawPixel(i, 50 - y2, true);
        }
      }

      display.Update();
    }
  }
};
