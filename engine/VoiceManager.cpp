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

  int8_t free_voice_idx = -1;

  int32_t oldest = 0;
  int8_t oldest_idx = -1;

  int32_t oldest_matching = -1;
  int8_t oldest_matching_idx = -1;

  int floor_note = 127;
  int8_t floor_idx = -1;

  int ceil_note = 0;
  int8_t ceil_idx = -1;

  for (int i = 0; i < number_of_voices_; i++) {
    if (voices_[i].state == Voice::VOICE_FREE) {
      free_voice_idx = i;
    }

    // Set oldest matching
    if (voices_[i].note.midi == new_note->midi &&
        voices_[i].age > oldest_matching) {
      oldest_matching = voices_[i].age;
      oldest_matching_idx = i;
    }

    // Set lowest
    if (voices_[i].note.midi <= floor_note) {
      floor_idx = i;
      floor_note = voices_[i].note.midi;
    }

    // Set highest
    if (voices_[i].note.midi >= ceil_note) {
      ceil_idx = i;
      ceil_note = voices_[i].note.midi;
    }

    if (voices_[i].state == Voice::VOICE_STEALABLE && voices_[i].age > oldest) {
      oldest = voices_[i].age;
      oldest_idx = i;
    }
  }

  // Try to assign oldest matching first.
  if (oldest_matching_idx != -1 && oldest_matching > 2) {
    free_voice = &(voices_[oldest_matching_idx]);
    free_voice->age = 0;

    return free_voice;
    // no need to reset phasor
  }

  // Try to assign oldest that is not the floor or ceiling
  if (oldest_idx != -1 && oldest_idx != floor_idx && oldest_idx != ceil_idx) {
    voices_[oldest_idx].StealVoice(*new_note);

    return NULL;
  }

  // Attempt to find a free voice.
  if (free_voice_idx != -1) {
    free_voice = &(voices_[free_voice_idx]);
    free_voice->age = 0;
    free_voice->ResetPhasor();

    return free_voice;
  }

  if (oldest_idx != -1) {
    voices_[oldest_idx].StealVoice(*new_note);

    return NULL;
  }

  return free_voice;
}

void VoiceManager::Process(float *left, float *right) {
  float output = 0.0f;
  float lfo_value = lfo_.Process();
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];
    if (voice.IsPlayable()) {
      voice.SetLFOValue(lfo_value);
      output += voice.Process();
    }
  }

  // Saturate the signal a bit
  output = tanh(volume_ * output);

  last_sample = output;

  *left = output;
  *right = output;
}

void VoiceManager::SetVolume(float new_vol) { volume_ = new_vol; };
