#include "VoiceManager.h"

Voice *VoiceManager::findFreeVoice(int midi_note) {
  Voice *free_voice = NULL;
  Voice *oldest_voice = NULL;
  uint32_t oldest = 0;

  for (int i = 0; i < number_of_voices_; i++) {
    // Replay the same note.
    if (voices_[i].note_number == midi_note) {
      free_voice = &(voices_[i]);
      free_voice->age = 0;
    } else {
      // Find an available voice
      if (!voices_[i].is_active) {
        free_voice = &(voices_[i]);
        free_voice->age = 0;
        // Alternatively, find the oldest playing voice
      } else {
        if (voices_[i].age > oldest) {
          oldest = voices_[i].age;
          oldest_voice = &(voices_[i]);
        }
      }
    }
  }
  if (free_voice == NULL && oldest_voice != NULL) {
    oldest_voice->age = 0;
    oldest_voice->ResetPhasor();
    return oldest_voice;
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

  float temp_vol = 1.f / (float)number_of_voices_;
  output = comp_.Process(output * temp_vol);

  last_sample = output;

  *left = output;
  *right = output;
}
