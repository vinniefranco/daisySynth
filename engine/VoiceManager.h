#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#pragma once
#include "Voice.h"

class VoiceManager {
private:
  float midi_[127];
  float lmin_ = logf(60.0f < 0.0000001f ? 0.0000001f : 60.0f);
  float lmax_ = logf(15000.0f);
  float volume_;
  static const int number_of_voices_ = 8;
  Voice voices_[number_of_voices_];
  WaveOsc lfo_;
  Voice *findFreeVoice(int noteNUmber);

public:
  void onNoteOn(int note_number, int velocity);
  void onNoteOff(int note_number, int velocity);
  void setFilterCutoff(float cutoff);
  inline void setVolume(float new_vol) { volume_ = new_vol; };
  void setFilterResonance(float resonance);
  float nextSample();
  void Process(float *left, float *right);
  void setSampleRate(float sample_rate) {
    EnvelopeGenerator::setSampleRate(sample_rate);
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.mOscOne.Init(sample_rate);
      voice.mOscOne.SetWaveform(voice.mOscOne.WAVE_SAW);
      voice.mOscOne.SetAmp(.5f);

      voice.mOscTwo.Init(sample_rate);
      voice.mOscTwo.SetWaveform(voice.mOscTwo.WAVE_SAW);
      voice.mOscTwo.SetAmp(.5f);
    }
    lfo_.Init(sample_rate);
    lfo_.SetWaveform(lfo_.WAVE_SIN);
    lfo_.SetAmp(0.9f);

    for (int x = 0; x < 127; ++x) {
      midi_[x] = daisysp::mtof(x);
    }
  }
  // inline void setLFOMode(POscillator::POscillatorMode mode) {
  //   mLFO.setMode(mode);
  // };
  inline void setLFOFrequency(float frequency) { lfo_.SetFreq(frequency); };
  // Functions to change a single voice
  static void setVolumeEnvelopeStageValue(
      Voice &voice, EnvelopeGenerator::EnvelopeStage stage, float value) {
    voice.mVolumeEnvelope.setStageValue(stage, value);
  }
  static void setFilterEnvelopeStageValue(
      Voice &voice, EnvelopeGenerator::EnvelopeStage stage, float value) {
    voice.mFilterEnvelope.setStageValue(stage, value);
  }
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

  static void setFilterEnvAmount(Voice &voice, float amount) {
    voice.setFilterEnvelopeAmount(amount);
  }
  static void setFilterLFOAmount(Voice &voice, float amount) {
    voice.setFilterLFOAmount(amount);
  }
};

#endif
