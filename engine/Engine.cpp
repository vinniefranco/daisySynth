#include "Engine.h"

void Engine::HandleAudioCallback(daisy::AudioHandle::OutputBuffer out,
                                 size_t size) {

  load_meter->OnBlockStart();
  voice_manager.SetFilterCutoff(GetCutoff());
  voice_manager.SetFilterResonance(GetRes());

  for (size_t i = 0; i < size; i++) {
    Process(&out[0][i], &out[1][i]);
  }
  load_meter->OnBlockEnd();
}

void Engine::Init(daisy::DaisySeed *seed, daisy::CpuLoadMeter *meter,
                  daisy::MidiUsbHandler *new_midi, float new_sample_rate) {
  hw = seed;

  sample_rate = new_sample_rate;
  load_meter = meter;
  midi = new_midi;

  load_meter->Init(sample_rate, hw->AudioBlockSize());

  // Config Display
  disp_cfg.driver_config.transport_config.pin_config.dc = hw->GetPin(11);
  disp_cfg.driver_config.transport_config.pin_config.reset = hw->GetPin(13);
  display.Init(disp_cfg);

  voice_manager.Init(sample_rate);
  voice_manager.SetLFOFrequency(0.03f);

  daisy::System::Delay(1000);
}

void Engine::SetCutoff(float value) {
  float cut_reading = value;
  if (value > 0.01f) {
    cutoff = 0.9f * (cut_reading - cutoff) + cutoff;
  } else {
    cutoff = 0.0f;
  }
}

void Engine::SetRes(int midi_value) { res = midiToFloat(midi_value) - 0.1f; }

float Engine::GetCutoff() { return cutoff; }

void Engine::SetVol(float new_x) { x = new_x; }

float Engine::GetVol() { return x; }

float Engine::GetRes() { return res; }

void Engine::Process(float *left, float *right) {
  voice_manager.Process(left, right);

  writeToUIBuffer(voice_manager.last_sample);
}

void Engine::writeToUIBuffer(float sample) {
  if (data_cursor >= 128) {
    data_cursor = 0;
    for (size_t i = 0; i < 128; i++) {
      screen_buffer[i] = data_buffer[i];
    }

  } else {
    data_buffer[data_cursor] = sample;
    data_cursor++;
  }
}

void Engine::ListenToMidi() {
  midi->Listen();

  while (midi->HasEvents()) {
    auto msg = midi->PopEvent();

    switch (msg.type) {
    case daisy::ControlChange: {
      auto cc = msg.AsControlChange();
      switch (cc.control_number) {

      case 1: {
        voice_manager.SetFilterLFOAmount(((float)cc.value / 127.f));
        break;
      }

      case 70: {

        if (cc.value == 0) {
          SetCutoff(0.0f);

        } else {
          float new_val = cc.value / 127.f;

          if (new_val > 0.001f) {
            new_val = daisysp::fmap(new_val, 0.01f, 1.f, daisysp::Mapping::LOG);
            SetCutoff(new_val);
          }
        }

        break;
      }

      case 71: {
        SetRes(cc.value);
        break;
      }

      case 72: {
        voice_manager.SetDetune((float)cc.value / 127.f);
        break;
      }

      case 73: {
        voice_manager.SetFilterEnvAmount((float)cc.value / 127.f);
        break;
      }

      case 74: {
        voice_manager.SetVolume((float)cc.value / 127.f);
        break;
      }

      case 75: {
        voice_manager.SetLFOFrequency(((float)cc.value / 127.f) * 10.f);
        break;
      }

      case 76: {
        voice_manager.SetOsc0Pitch(
            daisysp::fmap((float)cc.value / 127.f, 0.5f, 1.f));

        break;
      }

      case 77: {
        voice_manager.SetOscMix(
            daisysp::fmap((float)cc.value / 127.f, 0.0f, 1.f));

        break;
      }

      case 90: {
        voice_manager.SetVolumeAttack(daisysp::fmap(
            (float)cc.value / 127.f, 0.05f, 5.f, daisysp::Mapping::LOG));
        break;
      }

      case 91: {
        voice_manager.SetVolumeDecay(
            daisysp::fmap((float)cc.value / 127.f, 0.05f, 5.f));
        break;
      }

      case 92: {
        voice_manager.SetVolumeSustain(
            daisysp::fmax((float)cc.value / 127.f, 0.01f));
        break;
      }

      case 93: {
        voice_manager.SetVolumeRelease(
            daisysp::fmap((float)cc.value / 127.f, 0.1f, 5.f));
        break;
      }

      case 100: {
        voice_manager.SetFilterAttack(daisysp::fmap(
            (float)cc.value / 127.f, 0.005f, 5.f, daisysp::Mapping::LOG));
        break;
      }

      case 101: {
        voice_manager.SetFilterDecay(
            daisysp::fmap((float)cc.value / 127.f, 0.1f, 5.f));
        break;
      }

      case 102: {
        voice_manager.SetFilterSustain(
            daisysp::fmax((float)cc.value / 127.f, 0.01f));
        break;
      }

      case 103: {
        voice_manager.SetFilterRelease(
            daisysp::fmap((float)cc.value / 127.f, 0.1f, 5.f));
        break;
      }
      }
      break;
    }
    case daisy::NoteOn: {
      auto note_msg = msg.AsNoteOn();

      if (note_msg.velocity != 0) {
        SetVol(note_msg.note);
        voice_manager.OnNoteOn(note_msg.note, note_msg.velocity);
      }

    } break;
    case daisy::NoteOff: {
      auto note_msg = msg.AsNoteOff();

      voice_manager.OnNoteOff(note_msg.note, note_msg.velocity);

    } break;

    case daisy::PitchBend: {
      auto pitch_bend = msg.AsPitchBend();
      float bend = (float)pitch_bend.value / 8192;
      bend = powf(2, bend);

      voice_manager.SetPitchBend(bend);
      break;
    }
    default:
      break;
    }
  }
}

void Engine::tick() {
  if (ticker % 100 == 0) {
    const float avg_load = load_meter->GetAvgCpuLoad();

    display.Fill(false);

    sprintf(pot, "%02d", voice_manager.active_voices);
    display.SetCursor(2, 28);
    display.WriteString(pot, Font_6x8, true);

    uint8_t load = (avg_load * 127.f);

    // display.DrawRect(0, 26, 127, 36, true);
    display.DrawLine(14, 28, load + 14, 28, true);
    display.DrawLine(14, 30, load + 16, 30, true);
    display.DrawLine(14, 32, load + 16, 32, true);
    display.DrawLine(14, 34, load + 14, 34, true);

    float value = GetCutoff();

    float neg[128];
    float pos[128];
    for (size_t i = 0; i < 128; i++) {
      pos[i] = 0.0f;
      neg[i] = 0.0f;
      if (screen_buffer[i] > 0.0000f) {
        pos[i] = screen_buffer[i];
      } else if (screen_buffer[i] < 0.0000f) {
        neg[i] = fabs(screen_buffer[i]);
      }
    }

    for (size_t i = 0; i < 128; i++) {
      uint8_t y1 = (uint8_t)daisysp::fmap(pos[i] * 1.3f, 1.0f, 24.f,
                                          daisysp::Mapping::LINEAR);

      uint8_t y2 = (uint8_t)daisysp::fmap(neg[i] * 1.3f, 1.0f, 20.f,
                                          daisysp::Mapping::LINEAR);

      if (y1 > 0) {
        display.DrawPixel(i, 27 - y2, true);
      }
      if (y2 > 0) {
        display.DrawPixel(i, 35 + y1, true);
      }
    }

    display.Update();
  }
}
