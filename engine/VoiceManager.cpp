#include "VoiceManager.h"

Voice *VoiceManager::findFreeVoice(int midi_note) {
  Voice *free_voice = NULL;
  Voice *same_note = NULL;
  Voice *oldest_voice = NULL;
  uint32_t oldest = 0;

  for (int i = 0; i < number_of_voices_; i++) {
    if (!voices_[i].isActive) {
      free_voice = &(voices_[i]);
      free_voice->started_at = daisy::System::GetNow();
    } else {
      if (voices_[i].note_number == midi_note) {
        same_note = &(voices_[i]);
      } else if (voices_[i].started_at > oldest) {
        oldest = voices_[i].started_at;
        oldest_voice = &(voices_[i]);
      }
    }
  }
  if (free_voice == NULL) {
    if (same_note == NULL) {
      oldest_voice->started_at = daisy::System::GetNow();
      return oldest_voice;
    } else {
      same_note->started_at = daisy::System::GetNow();
      return same_note;
    }
  }

  return free_voice;
}

void VoiceManager::onNoteOn(int midi_note, int velocity) {
  Voice *voice = findFreeVoice(midi_note);
  if (!voice) {
    return;
  }
  voice->reset();
  voice->setNoteNumber(midi_note, midi_[midi_note]);
  voice->velocity = velocity;
  voice->isActive = true;
  voice->v_env.gate(true);
  voice->f_env.gate(true);
}
void VoiceManager::onNoteOff(int midi_note, int velocity) {
  // Find the voice with given note number
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];

    if (voice.isActive && voice.note_number == midi_note) {
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
    if (voice.isActive) {
      voice.setLFOValue(lfo_value);
      output += voice.nextSample();
    }
  }

  float temp_vol = 1.f / (float)number_of_voices_;
  output = comp_.Process(output * temp_vol);
  // output = chorus.Process(output);

  last_sample = output;

  // *left = chorus.GetLeft();
  // *right = chorus.GetRight();
  *left = output;
  *right = output;
}
