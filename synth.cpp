#include "../fpm/include/fpm/fixed.hpp"
#include "../fpm/include/fpm/math.hpp"

#include "VoiceManager.h"
#include "daisy_seed.h"
#include "synth_ui.h"

#define LEFT (i)
#define RIGHT (i + 1)

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
MidiUsbHandler midi;
CpuLoadMeter load_meter;
static VoiceManager voiceManager;
static SynthUI ui;
static Metro tick;

int melody[8] = {48 - 12, 52 - 12, 55 - 12, 59 - 12,
                 60 - 12, 59 - 12, 55 - 12, 52 - 12};

uint8_t counter = 0;
uint8_t inc = 0;

// MidiUartHandler midi;

void Tick(void *data) { ui.tick(); }

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  float output;

  load_meter.OnBlockStart();
  voiceManager.setFilterCutoff(ui.GetFreq());
  voiceManager.setFilterResonance(ui.GetRes());

  for (size_t i = 0; i < size; i += 2) {
    if (tick.Process()) {
      if (inc % 2 == 0) {
        voiceManager.onNoteOn(melody[counter % 8], 80);
      } else {
        voiceManager.onNoteOff(melody[counter % 8], 80);
        counter++;
      }

      inc++;
    }

    output = voiceManager.nextSample();

    out[LEFT] = output;
    out[RIGHT] = output;
  }
  load_meter.OnBlockEnd();
}

int main(void) {
  // Setup Seed
  float sample_rate;
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(4); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  sample_rate = hw.AudioSampleRate();

  const int num_adc_channels = 2;

  // Init ADC
  AdcChannelConfig adc_config[num_adc_channels];
  adc_config[0].InitSingle(daisy::seed::A4);
  adc_config[1].InitSingle(daisy::seed::A5);
  hw.adc.Init(adc_config, num_adc_channels);
  hw.adc.Start();

  // Start Serial LOG
  hw.StartLog();

  TimerHandle tim5;
  TimerHandle::Config tim_config;

  tim_config.periph = TimerHandle::Config::Peripheral::TIM_5;
  tim_config.enable_irq = true;

  auto tim_target_freq = 256;
  auto time_base_req = System::GetPClk2Freq();
  tim_config.period = time_base_req / tim_target_freq;
  tim5.Init(tim_config);
  tim5.SetCallback(Tick);

  // Configure
  tick.Init(8.0f, sample_rate);

  // Initialize UI
  ui.Init(&hw, &load_meter, daisy::seed::D17, daisy::seed::D16,
          daisy::seed::D15);

  voiceManager.setSampleRate(sample_rate);

  MidiUsbHandler::Config midi_cfg;
  midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
  midi.Init(midi_cfg);

  load_meter.Init(sample_rate, hw.AudioBlockSize());

  // Start Callback
  hw.StartAudio(AudioCallback);

  tim5.Start();
  while (1) {
    midi.Listen();

    while (midi.HasEvents()) {
      auto msg = midi.PopEvent();

      switch (msg.type) {
      case NoteOn: {
        auto note_msg = msg.AsNoteOn();

        if (note_msg.velocity != 0)
          voiceManager.onNoteOn(note_msg.note, note_msg.velocity);

      } break;
      case NoteOff: {
        auto note_msg = msg.AsNoteOff();

        voiceManager.onNoteOff(note_msg.note, note_msg.velocity);

      } break;
      default:
        int type = msg.type;

        break;
      }
    }
  }
}
