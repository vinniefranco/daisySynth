#include "VoiceManager.h"

Voice *VoiceManager::findFreeVoice() {
  Voice *freeVoice = NULL;
  for (int i = 0; i < NumberOfVoices; i++) {
    if (!voices[i].isActive) {
      freeVoice = &(voices[i]);
      break;
    }
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
  // voice->mVolumeEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
  // voice->mFilterEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
}
void VoiceManager::onNoteOff(int noteNumber, int velocity) {
  // Find the voice with given note number
  for (int i = 0; i < NumberOfVoices; i++) {
    Voice &voice = voices[i];
    if (voice.isActive && voice.mNoteNumber == noteNumber) {
      voice.setFree();
      voice.reset();
      // voice.mVolumeEnvelope.enterStage(
      //     EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
      // voice.mFilterEnvelope.enterStage(
      //     EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
    }
  }
}
float VoiceManager::nextSample() {
  float output = 0.0;
  // float lfoValue = mLFO.nextSample();
  for (int i = 0; i < NumberOfVoices; i++) {
    Voice &voice = voices[i];
    // voice.setLFOValue(lfoValue);
    output += voice.nextSample();
  }
  return output;
}
