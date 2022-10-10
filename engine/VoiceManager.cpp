#include "VoiceManager.h"

#include "arm_math.h"
#include "sys/system.h"

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
  voice->mVelocity = velocity;
  voice->isActive = true;
  voice->mVolumeEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
  voice->mFilterEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
}
void VoiceManager::onNoteOff(int midi_note, int velocity) {
  // Find the voice with given note number
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];

    if (voice.isActive && voice.note_number == midi_note) {
      voice.mVolumeEnvelope.enterStage(
          EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
      voice.mFilterEnvelope.enterStage(
          EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
    }
  }
}

void VoiceManager::setFilterCutoff(float cutoff) {
  float new_cutoff = expf(cutoff * (lmax_ - lmin_) + lmin_);
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];
    voice.flt.SetFreq(new_cutoff);
  }
}

void VoiceManager::setFilterResonance(float cutoff) {
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];
    voice.flt.SetRes(cutoff);
  }
}

float VoiceManager::nextSample() {
  float output = 0.0;
  float lfo_value = lfo_.Process();
  for (int i = 0; i < number_of_voices_; i++) {
    Voice &voice = voices_[i];
    if (voice.isActive) {
      voice.setLFOValue(lfo_value);
      output += voice.nextSample();
    }
  }
  return output / number_of_voices_ * volume_;
}

void VoiceManager::Process(float *left, float *right) {
  float output = nextSample();
  // Quick hard clipping
  if (output > 1.0f)
    output = 1.0f;

  if (output < -1.0f)
    output = -1.0f;

  *left = *right = output;
}
