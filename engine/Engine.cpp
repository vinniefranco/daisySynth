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
        voice_manager.SetVolumeAttack(daisysp::fmap((float)cc.value / 127.f,
                                                    0.05f, 5.f,
                                                    daisysp::Mapping::LOG) *
                                      sample_rate);
        break;
      }

      case 91: {
        voice_manager.SetVolumeDecay(
            daisysp::fmap((float)cc.value / 127.f, 0.05f, 5.f) * sample_rate);
        break;
      }

      case 92: {
        voice_manager.SetVolumeSustain(
            daisysp::fmax((float)cc.value / 127.f, 0.01f));
        break;
      }

      case 93: {
        voice_manager.SetVolumeRelease(
            daisysp::fmap((float)cc.value / 127.f, 0.1f, 5.f) * sample_rate);
        break;
      }

      case 100: {
        voice_manager.SetFilterAttack(daisysp::fmap((float)cc.value / 127.f,
                                                    0.005f, 5.f,
                                                    daisysp::Mapping::LOG) *
                                      sample_rate);
        break;
      }

      case 101: {
        voice_manager.SetFilterDecay(
            daisysp::fmap((float)cc.value / 127.f, 0.1f, 5.f) * sample_rate);
        break;
      }

      case 102: {
        voice_manager.SetFilterSustain(
            daisysp::fmax((float)cc.value / 127.f, 0.01f));
        break;
      }

      case 103: {
        voice_manager.SetFilterRelease(
            daisysp::fmap((float)cc.value / 127.f, 0.1f, 5.f) * sample_rate);
        break;
      }
      }
      break;
    }
    case daisy::NoteOn: {
      auto note_msg = msg.AsNoteOn();

      if (note_msg.velocity != 0)
        voice_manager.OnNoteOn(note_msg.note, note_msg.velocity);

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
