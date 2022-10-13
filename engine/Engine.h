#pragma once

#include "../table_encoder.h"
#include "VoiceManager.h"
#include <cstdint>
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
  float cutoff = 0.0f;
  float res = 0.0f;
  uint16_t ticker = 0;
  TableEncoder e;
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

  void Init(daisy::DaisySeed *seed, daisy::CpuLoadMeter *meter, daisy::Pin clk,
            daisy::Pin data, daisy::Pin swth, float sample_rate) {
    hw = seed;
    load_meter = meter;
    // Config Display
    disp_cfg.driver_config.transport_config.pin_config.dc = hw->GetPin(11);
    disp_cfg.driver_config.transport_config.pin_config.reset = hw->GetPin(13);
    display.Init(disp_cfg);

    voice_manager.setSampleRate(sample_rate);
    voice_manager.setLFOFrequency(0.03f);

    e.Init(clk, data, swth);

    daisy::System::Delay(1000);
  }

  inline void SetCutoff(int midi_value) { cutoff = midiToFloat(midi_value); }

  inline void SetRes(int midi_value) { res = midiToFloat(midi_value) - 0.1f; }

  inline float GetCutoff() { return cutoff; }

  inline float GetVol() { return x; }

  inline float GetRes() { return res; }

  int8_t Read() { return e.Read(); }

  void tick() {
    switch (Read()) {
    case 1:
      if (x <= 1.f) {
        x += 0.05f;
      }
      break;

    case -1:
      if (x >= 0.0f) {
        x -= 0.05f;
      }
      break;
    default:
      break;
    }

    float cut_reading = hw->adc.GetFloat(0);
    if (abs(cut_reading - last_cutoff_read) > 0.01f) {
      cutoff = 0.9f * (cut_reading - cutoff) + cutoff;
    }

    float res_reading = hw->adc.GetFloat(1) - 0.01f;
    if (abs(res_reading - last_res_read) > 0.01f) {
      res = 0.9f * (res_reading - res) + res;
    }

    if (ticker % 100 == 0) {
      const float avg_load = load_meter->GetAvgCpuLoad();

      display.Fill(false);

      sprintf(strbuff2, "LOAD:" FLT_FMT3, FLT_VAR3(avg_load * 100.0f));
      display.SetCursor(0, 0);
      display.WriteString(strbuff2, Font_6x8, true);

      float value = GetCutoff();

      uint_fast8_t position = value * 120.0f;

      sprintf(pot, "x:" FLT_FMT3, FLT_VAR3(x));
      display.SetCursor(0, 52);
      display.WriteString(pot, Font_6x8, true);

      for (size_t i = 0; i < 128; i++) {
        uint8_t y = (uint8_t)daisysp::fmap(screen_buffer[i] + 1.0f, 1.0f, 64.f,
                                           daisysp::Mapping::LOG);
        display.DrawPixel(i, 74 - y, true);
      }

      // display.DrawLine(0, 20, position, 20, true);
      // display.DrawLine(position, 20, position + 8, 40, true);

      display.Update();

      voice_manager.setVolume(x);
    }

    last_cutoff_read = cut_reading;
  }
};
