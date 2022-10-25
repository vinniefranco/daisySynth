#include "VoiceManager.h"

Voice *VoiceManager::findFreeVoice(int midi_note) {
  Voice *free_voice = NULL;
  uint32_t oldest = 0;
  uint8_t oldest_idx = -1;

  for (int i = 0; i < number_of_voices_; i++) {
    if (!voices_[i].is_active) {
      free_voice = &(voices_[i]);
      free_voice->age = 0;
    }
    if (voices_[i].age > oldest && voices_[i].is_stealable) {
      oldest = voices_[i].age;
      oldest_idx = i;
    }
  }
  if (free_voice == NULL) {
    free_voice = &(voices_[oldest_idx]);
    free_voice->age = 0;
    free_voice->ResetPhasor();
  }

  return free_voice;
}

void VoiceManager::Process(float *left, float *right) {
  float output = 0.0f;
  float lfo_value = lfo_.Process();
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];
    if (voice.is_active) {
      voice.SetLFOValue(lfo_value);
      output += voice.Process();
    }
  }

  output = volume_ * output;

  last_sample = output;

  *left = output;
  *right = output;
}
