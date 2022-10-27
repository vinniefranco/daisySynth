#pragma once

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
  float cutoff = 0.9f;
  float res = 0.0f;
  uint16_t ticker = 0;
  daisy::DaisySeed *hw;
  daisy::MidiUsbHandler *midi;
  daisy::CpuLoadMeter *load_meter;
  SynthOledDisplay::Config disp_cfg;
  SynthOledDisplay display;
  float sample_rate;

  char pot[128];
  char strbuff2[128];

  float midiToFloat(int midi_value) { return (float)midi_value / 127.f; }

public:
  VoiceManager voice_manager;

  Engine() {}
  ~Engine() {}

  void HandleAudioCallback(daisy::AudioHandle::OutputBuffer out, size_t size);
  void Process(float *left, float *right);
  void writeToUIBuffer(float sample);
  void ListenToMidi();
  void tick();

  void Init(daisy::DaisySeed *seed, daisy::CpuLoadMeter *meter,
            daisy::MidiUsbHandler *new_midi, float new_sample_rate);

  inline void SetCutoff(float value) {
    float cut_reading = value;
    if (value > 0.01f) {
      cutoff = 0.9f * (cut_reading - cutoff) + cutoff;
    } else {
      cutoff = 0.0f;
    }
  }

  inline void SetRes(int midi_value) { res = midiToFloat(midi_value) - 0.1f; }

  inline float GetCutoff() { return cutoff; }

  inline void SetVol(float new_x) { x = new_x; }
  inline float GetVol() { return x; }

  inline float GetRes() { return res; }
};
