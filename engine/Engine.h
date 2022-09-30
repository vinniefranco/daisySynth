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

  void Init(daisy::DaisySeed *seed, daisy::CpuLoadMeter *meter,
            daisy::MidiUsbHandler *new_midi, float new_sample_rate);

  float GetCutoff();
  float GetRes();
  float GetVol();
  void HandleAudioCallback(daisy::AudioHandle::OutputBuffer out, size_t size);
  void ListenToMidi();
  void Process(float *left, float *right);
  void SetCutoff(float value);
  void SetRes(int midi_value);
  void SetVol(float new_x);
  void tick();
  void writeToUIBuffer(float sample);
};
