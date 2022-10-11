#include "daisy_seed.h"

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
  voiceManager.setFilterCutoff(ui.GetCutoff());
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
      case ControlChange: {
        auto cc = msg.AsControlChange();
        switch (cc.control_number) {

        case 70: {
          ui.SetCutoff(cc.value);
          break;
        }

        case 71: {
          ui.SetRes(cc.value);
          break;
        }

        case 72: {
          voiceManager.setDetune((float)cc.value / 127.f);
          break;
        }

        case 73: {
          voiceManager.setFilterEnvAmount((float)cc.value / 127.f);
          break;
        }

        case 74: {
          voiceManager.setFilterLFOAmount(((float)cc.value / 127.f) / 2.f);
          break;
        }

        case 75: {
          voiceManager.setLFOFrequency(((float)cc.value / 127.f) * 10.f);
          break;
        }

        case 90: {
          voiceManager.setVolumeEnvelopeStageValue(
              VoiceManager::ATTACK,
              daisysp::fmap((float)cc.value / 127.f, 0.01f, 4.f,
                            daisysp::Mapping::LOG) *
                  sample_rate);
          break;
        }

        case 91: {
          voiceManager.setVolumeEnvelopeStageValue(
              VoiceManager::DECAY,
              daisysp::fmap((float)cc.value / 127.f, 0.1f, 2.f) * sample_rate);
          break;
        }

        case 92: {
          voiceManager.setVolumeEnvelopeStageValue(
              VoiceManager::SUSTAIN,
              daisysp::fmax((float)cc.value / 127.f, 0.01f));
          break;
        }

        case 93: {
          voiceManager.setVolumeEnvelopeStageValue(
              VoiceManager::RELEASE,
              daisysp::fmap((float)cc.value / 127.f, 0.1f, 4.f) * sample_rate);
          break;
        }

        case 100: {
          voiceManager.setFilterEnvelopeStageValue(
              VoiceManager::ATTACK,
              daisysp::fmap((float)cc.value / 127.f, 0.01f, 4.f,
                            daisysp::Mapping::LOG) *
                  sample_rate);
          break;
        }

        case 101: {
          voiceManager.setFilterEnvelopeStageValue(
              VoiceManager::DECAY,
              daisysp::fmap((float)cc.value / 127.f, 0.1f, 2.f) * sample_rate);
          break;
        }

        case 102: {
          voiceManager.setFilterEnvelopeStageValue(
              VoiceManager::SUSTAIN,
              daisysp::fmax((float)cc.value / 127.f, 0.01f));
          break;
        }

        case 103: {
          voiceManager.setFilterEnvelopeStageValue(
              VoiceManager::RELEASE,
              daisysp::fmap((float)cc.value / 127.f, 0.1f, 4.f) * sample_rate);
          break;
        }
        }
        break;
      }
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
