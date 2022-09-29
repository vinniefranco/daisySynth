#include "daisy_seed.h"
#include "daisysp.h"
#include "dev/oled_ssd130x.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

/** Typedef the OledDisplay to make syntax cleaner below
 *  This is a 4Wire SPI Transport controlling an 128x64 sized SSDD1306
 *
 *  There are several other premade test
 */
using SynthOledDisplay = OledDisplay<SSD130x4WireSpi128x64Driver>;

DaisySeed hw;
MidiUartHandler midi;
CpuLoadMeter load_meter;
SynthOledDisplay display;

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
  hw.Configure();
  hw.Init();

  // Start Serial LOG
  hw.StartLog();

  // Config Display
  SynthOledDisplay::Config disp_cfg;
  disp_cfg.driver_config.transport_config.pin_config.dc = hw.GetPin(11);
  disp_cfg.driver_config.transport_config.pin_config.reset = hw.GetPin(13);
  display.Init(disp_cfg);

  // Configure AUDIO/MIDI
  hw.SetAudioBlockSize(16); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  // MidiUartHandler::Config midi_cfg;
  // midi_cfg.transport_config.periph =
  // UartHandler::Config::Peripheral::USART_1; midi_cfg.transport_config.rx =
  // hw.GetPin(7); midi_cfg.transport_config.tx = hw.GetPin(6);
  // midi.Init(midi_cfg);

  load_meter.Init(hw.AudioSampleRate(), hw.AudioBlockSize());

  hw.StartAudio(AudioCallback);

  char strbuff1[128];
  char strbuff2[128];
  char strbuff3[128];
  while (1) {
    const float avg_load = load_meter.GetAvgCpuLoad();
    const float max_load = load_meter.GetMaxCpuLoad();
    const float min_load = load_meter.GetMinCpuLoad();

    System::Delay(600);

    display.Fill(false);

    display.SetCursor(0, 0);
    display.WriteString("CPU----------------", Font_7x10, true);

    sprintf(strbuff1, "Max:" FLT_FMT3, FLT_VAR3(max_load * 100.0f));
    display.SetCursor(0, 14);
    display.WriteString(strbuff1, Font_7x10, true);

    sprintf(strbuff2, "Avg:" FLT_FMT3, FLT_VAR3(avg_load * 100.0f));
    display.SetCursor(0, 28);
    display.WriteString(strbuff2, Font_7x10, true);

    sprintf(strbuff3, "Min:" FLT_FMT3, FLT_VAR3(min_load * 100.0f));
    display.SetCursor(0, 42);
    display.WriteString(strbuff3, Font_7x10, true);

    display.Update();

    // hw.PrintLine("Processing load %:");
    // System::Delay(1000);

    // midi.Listen();

    // while (midi.HasEvents()) {
    //   hw.PrintLine("MIDI");
    //   auto msg = midi.PopEvent();

    //   switch (msg.type) {
    //   case NoteOn:
    //     hw.PrintLine("MIDI");
    //     break;
    //   default:
    //     break;
    //   }
    // }
  }
}
