#include "daisy_seed.h"

#include "engine/Engine.h"
#include "engine/VoiceManager.h"
#include <math.h>

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
MidiUsbHandler midi;
CpuLoadMeter load_meter;

/**
 * WAVETABLEs need to be setup globals in SDRAM and drilled down.
 */
static constexpr int MAX_SLOTS = 36;

static waveTable DSY_SDRAM_BSS saw_wt[MAX_SLOTS];
static int total_saw_slots = 0;

static waveTable DSY_SDRAM_BSS sqr_wt[MAX_SLOTS];
static int total_sqr_slots = 0;

static WaveSlot DSY_SDRAM_BSS wt_slots[2];

void zeroFillWaveTables() {
  for (int idx = 0; idx < MAX_SLOTS; idx++) {
    sqr_wt[idx].topFreq = saw_wt[idx].topFreq = 0;
    sqr_wt[idx].waveTableLen = saw_wt[idx].waveTableLen = 0;
    sqr_wt[idx].waveTable = saw_wt[idx].waveTable = 0;
  }
}

void buildWavetableSlots() {
  // Populate SDRAM WaveTables
  zeroFillWaveTables();

  // Slot 0 is a SAW
  sawOsc(saw_wt, &total_saw_slots, MAX_SLOTS);
  WaveSlot slot0 = {.wt = saw_wt, .wt_slots = total_saw_slots};
  wt_slots[0] = slot0;

  // Slot 1 is a SQUARE
  sqrOsc(sqr_wt, &total_sqr_slots, MAX_SLOTS);
  WaveSlot slot1 = {.wt = sqr_wt, .wt_slots = total_sqr_slots};
  wt_slots[1] = slot1;
}

static Engine engine;

void Tick(void *data) { engine.tick(); }

static void AudioCallback(AudioHandle::InputBuffer in,
                          AudioHandle::OutputBuffer out, size_t size) {
  engine.HandleAudioCallback(out, size);
}

int main(void) {
  // Setup Seed
  float sample_rate;
  hw.Configure();
  hw.Init();

  buildWavetableSlots();

  hw.SetAudioBlockSize(4); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  sample_rate = hw.AudioSampleRate();

  // Setup timer
  TimerHandle tim5;
  TimerHandle::Config tim_config;
  tim_config.periph = TimerHandle::Config::Peripheral::TIM_5;
  tim_config.enable_irq = true;
  auto tim_target_freq = 48;
  auto time_base_req = System::GetPClk2Freq();
  tim_config.period = time_base_req / tim_target_freq;
  tim5.Init(tim_config);
  tim5.SetCallback(Tick);

  // Init MIDI
  MidiUsbHandler::Config midi_cfg;
  midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
  midi.Init(midi_cfg);

  // Initialize UI
  engine.Init(&hw, &load_meter, &midi, sample_rate);

  // Set static slots
  engine.voice_manager.SetWavetable(wt_slots);

  // Start Callback
  hw.StartAudio(AudioCallback);

  tim5.Start();
  while (1) {
    engine.ListenToMidi();
  }
}
