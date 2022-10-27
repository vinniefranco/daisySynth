#include "VoiceManager.h"
#include <cstdint>

void VoiceManager::Init(float sample_rate) {
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];
    voice.Init(sample_rate, 0.5f);
  }
  lfo_.Init(sample_rate);
  lfo_.SetWaveform(lfo_.WAVE_SIN);
  lfo_.SetAmp(0.1f);

  for (uint8_t x = 0; x < 127; x++) {
    midi_[x] = daisysp::mtof(x);
    if (x == 0) {
      key_follow_[x] = 0.0f;
    } else {
      key_follow_[x] =
          daisysp::fmap((float)x / 127.f, 0.0001f, 0.5f, daisysp::Mapping::LOG);
    }
  }
}

Voice *VoiceManager::FindFreeVoice(Note *new_note) {
  Voice *free_voice = NULL;
  int32_t oldest = 0;
  int8_t oldest_idx = -1;

  for (int i = 0; i < number_of_voices_; i++) {
    if (voices_[i].state == Voice::VOICE_STEALABLE) {
      if (voices_[i].note.midi == new_note->midi) {
        free_voice = &(voices_[i]);
        free_voice->age = 0;
        break;
      }

      if (voices_[i].age > oldest) {
        oldest = voices_[i].age;
        oldest_idx = i;
      }
    }

    if (voices_[i].state == Voice::VOICE_FREE) {
      free_voice = &(voices_[i]);
      free_voice->age = 0;
      free_voice->ResetPhasor();
      break;
    }
  }

  if (free_voice == NULL && oldest_idx != -1) {
    voices_[oldest_idx].StealVoice(*new_note);
  }

  return free_voice;
}

void VoiceManager::Process(float *left, float *right) {
  float output = 0.0f;
  float lfo_value = lfo_.Process();
  active_voices = 0;
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];
    if (voice.IsPlayable()) {
      active_voices++;
      voice.SetLFOValue(lfo_value);
      output += voice.Process();
    }
  }

  // Saturate the signal a bit
  output = tanh(volume_ * output);

  if (buffer_throttle % 3 == 0) {
    last_sample = output;
  }
  buffer_throttle++;

  *left = output;
  *right = output;
}

void VoiceManager::SetVolume(float new_vol) { volume_ = new_vol; };
