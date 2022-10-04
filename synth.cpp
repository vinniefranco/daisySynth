#include "daisy_seed.h"
#include "synth_ui.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

DaisySeed hw;
static MoogLadder flt1, flt2, flt3;
static Oscillator osc1, osc2, osc3, lfo;
static SynthUI ui;
static Chorus chorus;

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
    saw2 = osc2.Process();
    saw3 = osc3.Process();

    flt1.SetRes(ui.GetRes());
    flt1.SetFreq(ui.GetFreq());

    flt2.SetRes(ui.GetRes());
    flt2.SetFreq(ui.GetFreq());

    flt3.SetRes(ui.GetRes());
    flt3.SetFreq(ui.GetFreq());

    output = (flt1.Process(saw1) + flt2.Process(saw2) + flt3.Process(saw3));

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
  hw.SetAudioBlockSize(16); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  sample_rate = hw.AudioSampleRate();

  const int num_adc_channels = 2;

  // Init ADC
  AdcChannelConfig adc_config[num_adc_channels];
  adc_config[0].InitSingle(A4);
  adc_config[1].InitSingle(A5);
  hw.adc.Init(adc_config, num_adc_channels);
  hw.adc.Start();

  // Start Serial LOG
  hw.StartLog();

  // Configure

  osc1.Init(sample_rate);
  osc1.SetWaveform(osc1.WAVE_POLYBLEP_SAW);
  osc1.SetAmp(0.20f);
  osc1.SetFreq(87.31f);

  osc2.Init(sample_rate);
  osc2.SetWaveform(osc2.WAVE_POLYBLEP_SAW);
  osc2.SetAmp(0.30f);
  osc2.SetFreq(43.65f);

  osc3.Init(sample_rate);
  osc3.SetWaveform(osc2.WAVE_POLYBLEP_SAW);
  osc3.SetAmp(0.10f);
  osc3.SetFreq(86.41f);

  flt1.Init(sample_rate);
  flt1.SetRes(0.2);
  flt1.SetFreq(440.0);

  flt2.Init(sample_rate);
  flt2.SetRes(0.2);
  flt2.SetFreq(440.0);

  flt3.Init(sample_rate);
  flt3.SetRes(0.2);
  flt3.SetFreq(440.0);

  lfo.Init(sample_rate);
  lfo.SetWaveform(Oscillator::WAVE_POLYBLEP_TRI);
  lfo.SetAmp(0.02);
  lfo.SetFreq(0.1);

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
