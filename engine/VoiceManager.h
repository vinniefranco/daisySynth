#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#pragma once

#include "Effects/chorus.h"
#include "sys/system.h"

#include "Voice.h"

class VoiceManager {
private:
  daisysp::Compressor comp_;
  daisysp::Chorus chorus;
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
    chorus.Init(sample_rate);
    chorus.SetLfoFreq(0.54f, 0.48f);
    chorus.SetFeedback(0.3f);
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

    voice->SetNoteNumber(midi_note, midi_[midi_note], velocity);
  }
  inline void onNoteOff(int midi_note, int velocity) {
    ForEachVoice(ClearNoteNumber(midi_note));
  }

  inline void setFilterCutoff(float cutoff) {
    ForEachVoice(flt.SetCutoff(cutoff));
  }

  inline void setFilterResonance(float resonance) {
    ForEachVoice(flt.SetResonance(resonance));
  }

  inline void SetWavetable(WaveSlot *wt_slots) {
    ForEachVoice(SetWavetable(wt_slots))
  }

  inline void SetPitchBend(float value) { ForEachVoice(SetPitchBend(value)); }

  inline void setLFOFrequency(float frequency) { lfo_.SetFreq(frequency); };

  inline void setOscMix(float value) { ForEachVoice(SetOscMix(value)); }

  inline void setOsc0Pitch(float value) {
    ForEachVoice(SetOscOnePitchAmount(value));
  }

  inline void setOsc1Pitch(float value) {
    ForEachVoice(SetOscTwoPitchAmount(value));
  }

  inline void setVolumeAttack(float value) {
    ForEachVoice(v_env.SetAttackRate(value));
  }

  inline void setVolumeDecay(float value) {
    ForEachVoice(v_env.SetDecayRate(value));
  }

  inline void setVolumeSustain(float value) {
    ForEachVoice(v_env.SetSustainLevel(value));
  }

  inline void setVolumeRelease(float value) {
    ForEachVoice(v_env.SetReleaseRate(value));
  }

  inline void setFilterAttack(float value) {
    ForEachVoice(f_env.SetAttackRate(value));
  }

  inline void setFilterDecay(float value) {
    ForEachVoice(f_env.SetDecayRate(value));
  }

  inline void setFilterSustain(float value) {
    ForEachVoice(f_env.SetSustainLevel(value));
  }

  inline void setFilterRelease(float value) {
    ForEachVoice(f_env.SetReleaseRate(value));
  }

  inline void setDetune(float value) { ForEachVoice(SetDetune(value)); }

  inline void setFilterEnvAmount(float amount) {
    ForEachVoice(SetFilterEnvelopeAmount(amount));
  }

  inline void setFilterLFOAmount(float amount) {
    ForEachVoice(SetFilterLFOAmount(amount));
  }
};

#endif
