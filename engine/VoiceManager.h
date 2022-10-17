#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#pragma once

#include "sys/system.h"

#include "Voice.h"

class VoiceManager {
private:
  daisysp::Compressor comp_;
  float midi_[127];
  float volume_;
  static const int number_of_voices_ = 16;
  Voice voices_[number_of_voices_];
  daisysp::Oscillator lfo_;

  Voice *findFreeVoice(int noteNUmber);

public:
  float last_sample = 0.0f;
  inline void setVolume(float new_vol) { volume_ = new_vol; };
  void Process(float *left, float *right);
  void setSampleRate(float sample_rate) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.Init(sample_rate, 0.5f);
    }
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

  inline void onNoteOn(int midi_note, int velocity) {
    Voice *voice = findFreeVoice(midi_note);
    if (!voice) {
      return;
    }

    ForEachVoice(age += 1);

    voice->setNoteNumber(midi_note, midi_[midi_note], velocity);
  }
  inline void onNoteOff(int midi_note, int velocity) {
    ForEachVoice(clearNoteNumber(midi_note));
  }

  inline void setFilterCutoff(float cutoff) {
    ForEachVoice(flt.setCutoff(cutoff));
  }

  inline void setFilterResonance(float resonance) {
    ForEachVoice(flt.setResonance(resonance));
  }

  inline void SetWavetable(waveTable *saw, int total_saw_slots, waveTable *sqr,
                           int total_sqr_slots) {
    ForEachVoice(SetWavetable(saw, total_saw_slots, sqr, total_sqr_slots))
  }

  inline void SetPitchBend(float value) { ForEachVoice(SetPitchBend(value)); }

  inline void setLFOFrequency(float frequency) { lfo_.SetFreq(frequency); };

  inline void setOscMix(float value) { ForEachVoice(setOscMix(value)); }

  inline void setOsc0Pitch(float value) {
    ForEachVoice(setOscOnePitchAmount(value));
  }

  inline void setOsc1Pitch(float value) {
    ForEachVoice(setOscTwoPitchAmount(value));
  }

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
