#include "daisy_seed.h"

#include "engine/Engine.h"
#include "engine/VoiceManager.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
MidiUsbHandler midi;
CpuLoadMeter load_meter;

/**
 * WAVETABLEs need to be setup globals in SDRAM and drilled down.
 */
static constexpr int max_slots = 18;
static waveTable DSY_SDRAM_BSS saw_wt[max_slots];
static int total_saw_slots = 0;

void zeroFillWaveTable() {
  for (int idx = 0; idx < max_slots; idx++) {
    saw_wt[idx].topFreq = 0;
    saw_wt[idx].waveTableLen = 0;
    saw_wt[idx].waveTable = 0;
  }
}

static Engine engine;

void Tick(void *data) { engine.tick(); }

static void AudioCallback(AudioHandle::InputBuffer in,
                          AudioHandle::OutputBuffer out, size_t size) {
  load_meter.OnBlockStart();
  engine.voice_manager.setFilterCutoff(engine.GetCutoff());
  engine.voice_manager.setFilterResonance(engine.GetRes());

  for (size_t i = 0; i < size; i++) {
    engine.Process(&out[0][i], &out[1][i]);
  }
  load_meter.OnBlockEnd();
}

int main(void) {
  // Setup Seed
  float sample_rate;
  hw.Configure();
  hw.Init();

  // Populate SDRAM WaveTables
  zeroFillWaveTable();

  sawOsc(saw_wt, &total_saw_slots, max_slots);
  hw.SetAudioBlockSize(4); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  sample_rate = hw.AudioSampleRate();

  const int num_adc_channels = 2;

  // Init ADC
  AdcChannelConfig adc_config[num_adc_channels];
  adc_config[0].InitSingle(daisy::seed::A4);

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
  engine.Init(&hw, &load_meter, daisy::seed::D17, daisy::seed::D16,
              daisy::seed::D15, sample_rate);

  engine.voice_manager.SetWavetable(saw_wt, total_saw_slots);

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

        case 1: {
          engine.voice_manager.setFilterLFOAmount(((float)cc.value / 127.f) /
                                                  2.f);
          break;
        }

        case 70: {
          engine.SetCutoff(cc.value);
          break;
        }

        case 71: {
          engine.SetRes(cc.value);
          break;
        }

        case 72: {
          engine.voice_manager.setDetune((float)cc.value / 127.f);
          break;
        }

        case 73: {
          engine.voice_manager.setFilterEnvAmount((float)cc.value / 127.f);
          break;
        }

        case 75: {
          engine.voice_manager.setLFOFrequency(((float)cc.value / 127.f) *
                                               10.f);
          break;
        }

        case 90: {
          engine.voice_manager.setVolumeAttack(
              daisysp::fmap((float)cc.value / 127.f, 0.01f, 4.f,
                            daisysp::Mapping::LOG) *
              sample_rate);
          break;
        }

        case 91: {
          engine.voice_manager.setVolumeDecay(
              daisysp::fmap((float)cc.value / 127.f, 0.1f, 3.f) * sample_rate);
          break;
        }

        case 92: {
          engine.voice_manager.setVolumeSustain(
              daisysp::fmax((float)cc.value / 127.f, 0.01f));
          break;
        }

        case 93: {
          engine.voice_manager.setVolumeRelease(
              daisysp::fmap((float)cc.value / 127.f, 0.1f, 4.f) * sample_rate);
          break;
        }

        case 100: {
          engine.voice_manager.setFilterAttack(
              daisysp::fmap((float)cc.value / 127.f, 0.01f, 4.f,
                            daisysp::Mapping::LOG) *
              sample_rate);
          break;
        }

        case 101: {
          engine.voice_manager.setFilterDecay(
              daisysp::fmap((float)cc.value / 127.f, 0.1f, 2.f) * sample_rate);
          break;
        }

        case 102: {
          engine.voice_manager.setFilterSustain(
              daisysp::fmax((float)cc.value / 127.f, 0.01f));
          break;
        }

        case 103: {
          engine.voice_manager.setFilterRelease(
              daisysp::fmap((float)cc.value / 127.f, 0.1f, 4.f) * sample_rate);
          break;
        }
        }
        break;
      }
      case NoteOn: {
        auto note_msg = msg.AsNoteOn();

        if (note_msg.velocity != 0)
          engine.voice_manager.onNoteOn(note_msg.note, note_msg.velocity);

      } break;
      case NoteOff: {
        auto note_msg = msg.AsNoteOff();

        engine.voice_manager.onNoteOff(note_msg.note, note_msg.velocity);

      } break;
      default:
        break;
      }
    }
  }
}
