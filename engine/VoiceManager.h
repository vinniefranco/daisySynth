#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#include "Noise/whitenoise.h"
#pragma once
#include "sys/system.h"

#include "Voice.h"

class VoiceManager {
private:
  daisysp::Compressor comp_;
  float midi_[127];
  float volume_;
  daisysp::DelayLine<float, 32> del_;
  static const int number_of_voices_ = 12;
  Voice voices_[number_of_voices_];
  daisysp::Oscillator lfo_;

  Voice *findFreeVoice(int noteNUmber);

public:
  enum EnvStage { ATTACK, DECAY, SUSTAIN, RELEASE };
  void onNoteOn(int note_number, int velocity);
  void onNoteOff(int note_number, int velocity);
  void setFilterCutoff(float cutoff);
  inline void setVolume(float new_vol) { volume_ = new_vol; };
  void setFilterResonance(float resonance);
  float nextSample();
  void Process(float *left, float *right);
  void setSampleRate(float sample_rate) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.Init(sample_rate, WaveOsc::WAVE_SAW, 0.5f);
    }
    comp_.Init(sample_rate);
    comp_.AutoMakeup(false);
    comp_.SetMakeup(10.0f);
    lfo_.Init(sample_rate);
    lfo_.SetWaveform(lfo_.WAVE_SIN);
    lfo_.SetAmp(0.9f);

    for (int x = 0; x < 127; ++x) {
      midi_[x] = daisysp::mtof(x);
    }
  }
  inline void setLFOFrequency(float frequency) { lfo_.SetFreq(frequency); };

  inline void setVolumeEnvelopeStageValue(VoiceManager::EnvStage stage,
                                          float value) {

    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      switch (stage) {
      case ATTACK: {
        voice.v_env.setAttackRate(value);

        break;
      }
      case DECAY: {
        voice.v_env.setDecayRate(value);
        break;
      }
      case SUSTAIN: {
        voice.v_env.setSustainLevel(value);
        break;
      }

      case RELEASE: {
        voice.v_env.setReleaseRate(value);
        break;
      }
      }
    }
  }

  inline void setFilterEnvelopeStageValue(VoiceManager::EnvStage stage,
                                          float value) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      switch (stage) {
      case ATTACK: {
        voice.f_env.setAttackRate(value);

        break;
      }
      case DECAY: {
        voice.f_env.setDecayRate(value);
        break;
      }
      case SUSTAIN: {
        voice.f_env.setSustainLevel(value);
        break;
      }

      case RELEASE: {
        voice.f_env.setReleaseRate(value);
        break;
      }
      }
    }
  }

  inline void setDetune(float value) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.setDetune(value);
    }
  }

  inline void setFilterEnvAmount(float amount) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.setFilterEnvelopeAmount(amount);
    }
  }

  inline void setFilterLFOAmount(float amount) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.setFilterLFOAmount(amount);
    }
  }
};

#endif
