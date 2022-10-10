#pragma once

#include "table_encoder.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unordered_map>

#include "daisy_seed.h"
#include "daisysp.h"
#include "dev/oled_ssd130x.h"
#include "per/gpio.h"

class SynthParam {
public:
  SynthParam(int);
  ~SynthParam();
};

namespace daisy {

using SynthOledDisplay = OledDisplay<SSD130x4WireSpi128x64Driver>;

class SynthUI {
private:
  float last_cutoff_read = 0.0f;
  float last_res_read = 0.0f;
  float x = 0.8f;
  float cutoff = 0.0f;
  float res = 0.0f;
  uint16_t ticker = 0;
  TableEncoder e;
  DaisySeed *hw;
  SynthOledDisplay::Config disp_cfg;
  SynthOledDisplay display;
  CpuLoadMeter *load_meter;

  char pot[128];
  char strbuff2[128];

  float midiToFloat(int midi_value) { return (float)midi_value / 127.f; }

public:
  SynthUI() {}
  ~SynthUI() {}

  void Init(DaisySeed *seed, CpuLoadMeter *meter, daisy::Pin clk,
            daisy::Pin data, daisy::Pin swth) {
    hw = seed;
    load_meter = meter;
    // Config Display
    disp_cfg.driver_config.transport_config.pin_config.dc = hw->GetPin(11);
    disp_cfg.driver_config.transport_config.pin_config.reset = hw->GetPin(13);
    display.Init(disp_cfg);

    e.Init(clk, data, swth);

    System::Delay(1000);
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

      sprintf(pot, "x:" FLT_FMT3, FLT_VAR3(cutoff));
      display.SetCursor(0, 52);
      display.WriteString(pot, Font_6x8, true);

      display.DrawLine(0, 20, position, 20, true);
      display.DrawLine(position, 20, position + 8, 40, true);

      display.Update();
    }

    last_cutoff_read = cut_reading;
  }
};

} // namespace daisy
