#include "daisy_seed.h"

#include "synth_ui.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

DaisySeed hw;
static MoogLadder flt;
static Oscillator osc1, osc2, osc3, lfo;
static SynthUI ui;

// MidiUartHandler midi;
// MidiUsbHandler midi;
CpuLoadMeter load_meter;

static void AudioCallback(AudioHandle::InputBuffer in,
                          AudioHandle::OutputBuffer out, size_t size) {
  float saw1, saw2, saw3, freq, output;

  load_meter.OnBlockStart();
  for (size_t i = 0; i < size; i++) {
    freq = lfo.Process();
    saw1 = osc1.Process() + freq;
    saw2 = osc2.Process() - freq;
    saw3 = osc3.Process();

    flt.SetFreq(ui.GetFreq());

    output = flt.Process(saw1 + saw2 + saw3);

    out[0][i] = output;
    out[1][i] = output;
  }
  load_meter.OnBlockEnd();
}

int main(void) {
  // Setup Seed
  float sample_rate;
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(4); // number of samples handled per callback
  // hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  sample_rate = hw.AudioSampleRate();

  // Init ADC
  AdcChannelConfig adc_config;
  adc_config.InitSingle(A4);
  hw.adc.Init(&adc_config, 1);
  hw.adc.Start();

  // Start Serial LOG
  hw.StartLog();

  // Configure

  osc1.Init(sample_rate);
  osc1.SetWaveform(osc1.WAVE_POLYBLEP_SAW);
  osc1.SetAmp(0.35f);
  osc1.SetFreq(86.31f);

  osc2.Init(sample_rate);
  osc2.SetWaveform(osc2.WAVE_POLYBLEP_SAW);
  osc2.SetAmp(0.35f);
  osc2.SetFreq(43.65f);

  osc3.Init(sample_rate);
  osc3.SetWaveform(osc2.WAVE_POLYBLEP_SAW);
  osc3.SetAmp(0.35f);
  osc3.SetFreq(87.31f + 0.09f);

  flt.Init(sample_rate);
  flt.SetRes(0.7);
  flt.SetFreq(440.0);

  lfo.Init(sample_rate);
  lfo.SetWaveform(Oscillator::WAVE_TRI);
  lfo.SetAmp(0.1);
  lfo.SetFreq(0.4);

  // Initialize UI
  ui.Init(&hw, &load_meter, D17, D16, D15);

  // MidiUartHandler::Config midi_cfg;
  // midi_cfg.transport_config.periph =
  // UartHandler::Config::Peripheral::USART_2; midi_cfg.transport_config.rx
  // = hw.GetPin(37); midi_cfg.transport_config.tx = hw.GetPin(36);
  // midi.Init(midi_cfg);
  // MidiUsbHandler::Config midi_cfg;
  // midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
  // midi.Init(midi_cfg);

  load_meter.Init(sample_rate, hw.AudioBlockSize());

  // Start Callback
  hw.StartAudio(AudioCallback);

  while (1) {
    ui.tick();
  }
}
