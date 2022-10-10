#include "daisy_seed.h"
#include <queue>

#include "engine/VoiceManager.h"
#include "synth_ui.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
MidiUsbHandler midi;
CpuLoadMeter load_meter;
static VoiceManager voiceManager;
static SynthUI ui;

// MidiUartHandler midi;

void Tick(void *data) {
  ui.tick();
  voiceManager.setVolume(ui.GetVol());
}

static void AudioCallback(AudioHandle::InputBuffer in,
                          AudioHandle::OutputBuffer out, size_t size) {
  load_meter.OnBlockStart();
  voiceManager.setFilterCutoff(ui.GetFreq());
  voiceManager.setFilterResonance(ui.GetRes());

  for (size_t i = 0; i < size; i++) {
    voiceManager.Process(&out[0][i], &out[1][i]);
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

  auto tim_target_freq = 48;
  auto time_base_req = System::GetPClk2Freq();
  tim_config.period = time_base_req / tim_target_freq;
  tim5.Init(tim_config);
  tim5.SetCallback(Tick);

  // Initialize UI
  ui.Init(&hw, &load_meter, daisy::seed::D17, daisy::seed::D16,
          daisy::seed::D15);

  voiceManager.setSampleRate(sample_rate);
  voiceManager.setLFOFrequency(0.03f);

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
        break;
      }
    }
  }
}
