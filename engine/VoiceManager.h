#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#pragma once

#include "Noise/whitenoise.h"
#include "sys/system.h"

#include "Voice.h"

class VoiceManager {
private:
  daisysp::Compressor comp_;
  float midi_[127];
  float volume_;
  static const int number_of_voices_ = 10;
  Voice voices_[number_of_voices_];
  daisysp::Oscillator lfo_;

  Voice *findFreeVoice(int noteNUmber);

public:
  float last_sample = 0.0f;
  enum EnvStage { ATTACK, DECAY, SUSTAIN, RELEASE };
  void onNoteOn(int note_number, int velocity);
  void onNoteOff(int note_number, int velocity);
  void setFilterCutoff(float cutoff);
  daisysp::Chorus chorus;
  inline void setVolume(float new_vol) { volume_ = new_vol; };
  void setFilterResonance(float resonance);
  void Process(float *left, float *right);
  void setSampleRate(float sample_rate) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.Init(sample_rate, 0.5f);
    }
    chorus.Init(sample_rate);
    chorus.SetLfoDepth(0.5f);
    chorus.SetDelay(1);
    chorus.SetFeedback(0.5f);
    comp_.Init(sample_rate);
    comp_.AutoMakeup(false);
    comp_.SetMakeup(14.0f);
    lfo_.Init(sample_rate);
    lfo_.SetWaveform(lfo_.WAVE_SIN);
    lfo_.SetAmp(0.1f);

    for (int x = 0; x < 127; ++x) {
      midi_[x] = daisysp::mtof(x);
    }
  }

#define ForEachVoice(expr)                                                     \
  for (int i = 0; i < number_of_voices_; i++) {                                \
    Voice &voice = voices_[i];                                                 \
    voice.expr;                                                                \
  }

  inline void SetWavetable(waveTable *wt, int total_slots) {
    ForEachVoice(SetWavetable(wt, total_slots))
  }

  inline void SetPitchBend(float value) { ForEachVoice(SetPitchBend(value)); }

  inline void setLFOFrequency(float frequency) { lfo_.SetFreq(frequency); };

  inline void setVolumeAttack(float value) {
    ForEachVoice(v_env.setAttackRate(value));
  }

  inline void setVolumeDecay(float value) {
    ForEachVoice(v_env.setDecayRate(value));
  }

  inline void setVolumeSustain(float value) {
    ForEachVoice(v_env.setSustainLevel(value));
  }

  inline void setVolumeRelease(float value) {
    ForEachVoice(v_env.setReleaseRate(value));
  }

  inline void setFilterAttack(float value) {
    ForEachVoice(f_env.setAttackRate(value));
  }

  inline void setFilterDecay(float value) {
    ForEachVoice(f_env.setDecayRate(value));
  }

  inline void setFilterSustain(float value) {
    ForEachVoice(f_env.setSustainLevel(value));
  }

  inline void setFilterRelease(float value) {
    ForEachVoice(f_env.setReleaseRate(value));
  }

  inline void setDetune(float value) { ForEachVoice(setDetune(value)); }

  inline void setFilterEnvAmount(float amount) {
    ForEachVoice(setFilterEnvelopeAmount(amount));
  }

  inline void setFilterLFOAmount(float amount) {
    ForEachVoice(setFilterLFOAmount(amount));
  }
};

#endif
