#include "VoiceManager.h"

Voice *VoiceManager::findFreeVoice(int midi_note) {
  Voice *free_voice = NULL;
  Voice *oldest_voice = NULL;
  uint32_t oldest = 0;

  for (int i = 0; i < number_of_voices_; i++) {
    // Replay the same note.
    if (voices_[i].note_number == midi_note) {
      free_voice = &(voices_[i]);
      free_voice->started_at = daisy::System::GetNow();
    } else {
      if (!voices_[i].is_active) {
        free_voice = &(voices_[i]);
        free_voice->started_at = daisy::System::GetNow();
      } else {
        if (voices_[i].started_at > oldest) {
          oldest = voices_[i].started_at;
          oldest_voice = &(voices_[i]);
        }
      }
    }
  }
  if (free_voice == NULL && oldest_voice != NULL) {
    oldest_voice->started_at = daisy::System::GetNow();
    oldest_voice->ResetPhasor();
    return oldest_voice;
  }

  return free_voice;
}

void VoiceManager::onNoteOn(int midi_note, int velocity) {
  Voice *voice = findFreeVoice(midi_note);
  if (!voice) {
    return;
  }

  if (voice->note_number != midi_note) {
    voice->reset();
    voice->ResetPhasor();
  }
  voice->setNoteNumber(midi_note, midi_[midi_note]);
  voice->velocity = velocity;
  voice->is_active = true;
  voice->v_env.gate(true);
  voice->f_env.gate(true);
}
void VoiceManager::onNoteOff(int midi_note, int velocity) {
  // Find the voice with given note number
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];

    if (voice.note_number == midi_note) {
      voice.v_env.gate(false);
      voice.f_env.gate(false);
    }
  }
}

void VoiceManager::setFilterCutoff(float cutoff) {
  ForEachVoice(flt.setCutoff(cutoff));
}

void VoiceManager::setFilterResonance(float resonance) {
  ForEachVoice(flt.setResonance(resonance));
}

void VoiceManager::Process(float *left, float *right) {
  float output = 0.0f;
  float lfo_value = lfo_.Process();
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];
    if (voice.is_active) {
      voice.setLFOValue(lfo_value);
      output += voice.nextSample();
    }
  }

  float temp_vol = 1.f / (float)number_of_voices_;
  output = comp_.Process(output * temp_vol);

  last_sample = output;

  *left = output;
  *right = output;
}
