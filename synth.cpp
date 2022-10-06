#include "VoiceManager.h"
#include "daisy_seed.h"
#include "synth_ui.h"

#define LEFT (i)
#define RIGHT (i + 1)

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

DaisySeed hw;
static VoiceManager voiceManager;
static MoogLadder flt;
static Oscillator osc;
static Adsr env;
static SynthUI ui;
static Metro tick;

int melody[5] = {43, 50, 53, 60, 65};
int counter = 0;

// MidiUartHandler midi;
// MidiUsbHandler midi;
CpuLoadMeter load_meter;

void Tick(void *data) { ui.tick(); }

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  float output;
  int back_key;

  load_meter.OnBlockStart();
  for (size_t i = 0; i < size; i += 2) {
    if (tick.Process()) {
      // voiceManager.onNoteOff(melody[counter - 1 % 4], 80);
      voiceManager.onNoteOn(melody[counter], 80);
      back_key = counter - 1 % 5;
      counter = 1 + counter % 5;

      voiceManager.onNoteOff(melody[back_key], 80);
    }

    // env_out = env.Process(gate);
    // osc.SetAmp(env_out);

    // saw = osc.Process();

    // flt.SetRes(ui.GetRes());
    // flt.SetFreq(ui.GetFreq());

    // output = flt.Process(saw);
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
  tick.Init(4.0f, sample_rate);

  // Initialize UI
  ui.Init(&hw, &load_meter, D17, D16, D15);

  voiceManager.setSampleRate(sample_rate);
  voiceManager.onNoteOn(40, 80);

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

  tim5.Start();
  while (1) {
  }
}
