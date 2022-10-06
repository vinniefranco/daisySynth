#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#pragma once
#include "Voice.h"

class VoiceManager {
private:
  static const int NumberOfVoices = 6;
  Voice voices[NumberOfVoices];
  // POscillator mLFO;
  Voice *findFreeVoice();

public:
  void onNoteOn(int noteNumber, int velocity);
  void onNoteOff(int noteNumber, int velocity);
  float nextSample();
  void setSampleRate(float sampleRate) {
    // EnvelopeGenerator::setSampleRate(sampleRate);
    for (int i = 0; i < NumberOfVoices; i++) {
      Voice &voice = voices[i];
      voice.mOscOne.Init(sampleRate);
      voice.mOscOne.SetWaveform(voice.mOscOne.WAVE_POLYBLEP_SAW);
      voice.mOscOne.SetAmp(.25);

      voice.mOscTwo.Init(sampleRate);
      voice.mOscTwo.SetWaveform(voice.mOscTwo.WAVE_POLYBLEP_SAW);
      voice.mOscTwo.SetAmp(.25);
    }
    // mLFO.setSampleRate(sampleRate);
  }
  // inline void setLFOMode(POscillator::POscillatorMode mode) {
  //   mLFO.setMode(mode);
  // };
  // inline void setLFOFrequency(float frequency) {
  //   mLFO.setFrequency(frequency);
  // };
  // Functions to change a single voice
  // static void setVolumeEnvelopeStageValue(
  //     Voice &voice, EnvelopeGenerator::EnvelopeStage stage, float value) {
  //   voice.mVolumeEnvelope.setStageValue(stage, value);
  // }
  // static void setFilterEnvelopeStageValue(
  //     Voice &voice, EnvelopeGenerator::EnvelopeStage stage, float value) {
  //   voice.mFilterEnvelope.setStageValue(stage, value);
  // }
  // static void setOscillatorMode(Voice &voice, int oscillatorNumber,
  //                               POscillator::POscillatorMode mode) {
  //   switch (oscillatorNumber) {
  //   case 1:
  //     voice.mOscOne.setMode(mode);
  //     break;
  //   case 2:
  //     voice.mOscTwo.setMode(mode);
  //     break;
  //   }
  // }
  // static void setOscillatorPitchMod(Voice &voice, int oscillatorNumber,
  //                                   float amount) {
  //   switch (oscillatorNumber) {
  //   case 1:
  //     voice.setOscOnePitchAmount(amount);
  //     break;
  //   case 2:
  //     voice.setOscTwoPitchAmount(amount);
  //     break;
  //   }
  // }
  static void setOscillatorMix(Voice &voice, float value) {
    voice.setOscMix(value);
  }
  // static void setFilterCutoff(Voice &voice, float cutoff) {
  //   voice.mFilter.setCutoff(cutoff);
  // }
  // static void setFilterResonance(Voice &voice, float resonance) {
  //   voice.mFilter.setResonance(resonance);
  // }
  // static void setFilterMode(Voice &voice, Filter::FilterMode mode) {
  //   voice.mFilter.setFilterMode(mode);
  // }

  // static void setFilterEnvAmount(Voice &voice, double amount) {
  //   voice.setFilterEnvelopeAmount(amount);
  // }
  // static void setFilterLFOAmount(Voice &voice, double amount) {
  //   voice.setFilterLFOAmount(amount);
  // }
};

#endif
