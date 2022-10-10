#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#include "Noise/whitenoise.h"
#pragma once
#include "Utility/delayline.h"
#include "sys/system.h"

#include "Voice.h"

class VoiceManager {
private:
  daisysp::Compressor comp_;
  float midi_[127];
  float lmin_ = logf(60.0f < 0.0000001f ? 0.0000001f : 60.0f);
  float lmax_ = logf(15000.0f);
  float volume_;
  daisysp::DelayLine<float, 32> del_;
  static const int number_of_voices_ = 8;
  Voice voices_[number_of_voices_];
  daisysp::Oscillator lfo_;

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
      voice.Init(sample_rate, WaveOsc::WAVE_SAW, 0.5f);
    }
    comp_.Init(sample_rate);
    comp_.AutoMakeup(false);
    comp_.SetMakeup(10.0f);
    del_.Init();
    size_t delay = 2;
    del_.SetDelay(delay);
    del_.Write(0.0f);
    lfo_.Init(sample_rate);
    lfo_.SetWaveform(lfo_.WAVE_SIN);
    lfo_.SetAmp(0.9f);

    for (int x = 0; x < 127; ++x) {
      midi_[x] = daisysp::mtof(x);
    }
  }
  inline void setLFOFrequency(float frequency) { lfo_.SetFreq(frequency); };

  inline void
  setVolumeEnvelopeStageValue(EnvelopeGenerator::EnvelopeStage stage,
                              float value) {

    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.mVolumeEnvelope.setStageValue(stage, value);
    }
  }

  inline void
  setFilterEnvelopeStageValue(EnvelopeGenerator::EnvelopeStage stage,
                              float value) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.mFilterEnvelope.setStageValue(stage, value);
    }
  }

  inline void setFilterEnvAmount(float amount) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.setFilterEnvelopeAmount(amount);
    }
  }
  static void setFilterLFOAmount(Voice &voice, float amount) {
    voice.setFilterLFOAmount(amount);
  }
};

#endif
