#include "daisy_seed.h"

#include "synth_ui.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

DaisySeed hw;
// MidiUartHandler midi;
// MidiUsbHandler midi;
CpuLoadMeter load_meter;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out,
                   size_t size) {
  load_meter.OnBlockStart();
  for (size_t i = 0; i < size; i++) {
    out[0][i] = in[0][i];
    out[1][i] = in[1][i];
  }
  load_meter.OnBlockEnd();
}

int main(void) {
  // Setup Seed
  hw.Configure();
  hw.Init();

  // Start Serial LOG
  hw.StartLog();

  // Initialize UI
  SynthUI ui;

  ui.Init(&hw, &load_meter, D17, D16, D15);

  // Configure AUDIO/MIDI
  hw.SetAudioBlockSize(16); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  // MidiUartHandler::Config midi_cfg;
  // midi_cfg.transport_config.periph =
  // UartHandler::Config::Peripheral::USART_2; midi_cfg.transport_config.rx
  // = hw.GetPin(37); midi_cfg.transport_config.tx = hw.GetPin(36);
  // midi.Init(midi_cfg);
  // MidiUsbHandler::Config midi_cfg;
  // midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
  // midi.Init(midi_cfg);

  load_meter.Init(hw.AudioSampleRate(), hw.AudioBlockSize());

  hw.StartAudio(AudioCallback);

  while (1) {
    ui.tick();
  }
}
