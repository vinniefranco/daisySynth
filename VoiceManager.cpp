#include "VoiceManager.h"

#include "sys/system.h"

Voice *VoiceManager::findFreeVoice() {
  Voice *freeVoice = NULL;
  Voice *stolenVoice = NULL;
  uint32_t oldest = 0;

  for (int i = 0; i < NumberOfVoices; i++) {
    if (!voices[i].isActive) {
      freeVoice = &(voices[i]);
      freeVoice->started_at = daisy::System::GetNow();
    } else {
      if (voices[i].started_at > oldest) {
        oldest = voices[i].started_at;
        stolenVoice = &(voices[i]);
      }
    }
  }
  if (freeVoice == NULL) {
    stolenVoice->started_at = daisy::System::GetNow();
    return stolenVoice;
  }

  return freeVoice;
}

void VoiceManager::onNoteOn(int noteNumber, int velocity) {
  Voice *voice = findFreeVoice();
  if (!voice) {
    return;
  }
  voice->reset();
  voice->setNoteNumber(noteNumber);
  voice->mVelocity = velocity;
  voice->isActive = true;
  voice->mVolumeEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
  voice->mFilterEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
}
void VoiceManager::onNoteOff(int noteNumber, int velocity) {
  // Find the voice with given note number
  for (int i = 0; i < NumberOfVoices; i++) {
    Voice &voice = voices[i];

    if (voice.isActive && voice.mNoteNumber == noteNumber) {
      voice.mVolumeEnvelope.enterStage(
          EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
      voice.mFilterEnvelope.enterStage(
          EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
    }
  }
}

void VoiceManager::setFilterCutoff(float cutoff) {
  float newCutoff = expf(cutoff * (lmax_ - lmin_) + lmin_);
  for (int i = 0; i < NumberOfVoices; i++) {
    Voice &voice = voices[i];
    voice.mFilter.SetFreq(newCutoff);
  }
}

void VoiceManager::setFilterResonance(float cutoff) {
  for (int i = 0; i < NumberOfVoices; i++) {
    Voice &voice = voices[i];
    voice.mFilter.SetRes(cutoff);
  }
}

float VoiceManager::nextSample() {
  float output = 0.0;
  float lfoValue = mLFO.Process();
  for (int i = 0; i < NumberOfVoices; i++) {
    Voice &voice = voices[i];
    if (voice.isActive) {
      voice.setLFOValue(lfoValue);
      output += voice.nextSample();
    }
  }
  return output;
}
