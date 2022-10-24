#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#pragma once

#include "Effects/chorus.h"
#include "sys/system.h"

#include "Voice.h"

class VoiceManager {
private:
  float midi_[127];
  float key_follow_[127];
  float volume_;
  static const int number_of_voices_ = 16;
  Voice voices_[number_of_voices_];
  daisysp::Oscillator lfo_;

  Voice *findFreeVoice(int noteNUmber);

public:
  float last_sample = 0.0f;

  VoiceManager(){};
  ~VoiceManager(){};

  void Process(float *left, float *right);

  inline void SetVolume(float new_vol) { volume_ = new_vol; };
  inline void Init(float sample_rate) {
    for (int i = 0; i < number_of_voices_; i++) {
      Voice &voice = voices_[i];
      voice.Init(sample_rate, 0.5f);
    }
    lfo_.Init(sample_rate);
    lfo_.SetWaveform(lfo_.WAVE_SIN);
    lfo_.SetAmp(0.1f);

    for (int x = 0; x < 127; x++) {
      midi_[x] = daisysp::mtof(x);
      key_follow_[x] = daisysp::fmap((float)x, 0.001f, 0.16f);
    }
  }

#define ForEachVoice(expr)                                                     \
  for (int i = 0; i < number_of_voices_; i++) {                                \
    Voice &voice = voices_[i];                                                 \
    voice.expr;                                                                \
  }

  inline void OnNoteOn(int midi_note, int velocity) {
    Voice *voice = findFreeVoice(midi_note);
    if (!voice) {
      return;
    }

    ForEachVoice(age += 1);

    voice->SetNoteNumber(
        midi_note, midi_[midi_note],
        daisysp::fmap((float)velocity, 0.001f, 1.f, daisysp::Mapping::LOG),
        key_follow_[midi_note]);
  }
  inline void OnNoteOff(int midi_note, int velocity) {
    ForEachVoice(ClearNoteNumber(midi_note));
  }

  inline void SetFilterCutoff(float cutoff) {
    ForEachVoice(flt.SetCutoff(cutoff));
  }

  inline void SetFilterResonance(float resonance) {
    ForEachVoice(flt.SetResonance(resonance));
  }

  inline void SetWavetable(WaveSlot *wt_slots) {
    ForEachVoice(SetWavetable(wt_slots))
  }

  inline void SetPitchBend(float value) { ForEachVoice(SetPitchBend(value)); }

  inline void SetLFOFrequency(float frequency) { lfo_.SetFreq(frequency); };

  inline void SetOscMix(float value) { ForEachVoice(SetOscMix(value)); }

  inline void SetOsc0Pitch(float value) {
    ForEachVoice(SetOscOnePitchAmount(value));
  }

  inline void SetOsc1Pitch(float value) {
    ForEachVoice(SetOscTwoPitchAmount(value));
  }

  inline void SetVolumeAttack(float value) {
    ForEachVoice(v_env.SetAttackRate(value));
  }

  inline void SetVolumeDecay(float value) {
    ForEachVoice(v_env.SetDecayRate(value));
  }

  inline void SetVolumeSustain(float value) {
    ForEachVoice(v_env.SetSustainLevel(value));
  }

  inline void SetVolumeRelease(float value) {
    ForEachVoice(v_env.SetReleaseRate(value));
  }

  inline void SetFilterAttack(float value) {
    ForEachVoice(f_env.SetAttackRate(value));
  }

  inline void SetFilterDecay(float value) {
    ForEachVoice(f_env.SetDecayRate(value));
  }

  inline void SetFilterSustain(float value) {
    ForEachVoice(f_env.SetSustainLevel(value));
  }

  inline void SetFilterRelease(float value) {
    ForEachVoice(f_env.SetReleaseRate(value));
  }

  inline void SetDetune(float value) { ForEachVoice(SetDetune(value)); }

  inline void SetFilterEnvAmount(float amount) {
    ForEachVoice(SetFilterEnvelopeAmount(amount));
  }

  inline void SetFilterLFOAmount(float amount) {
    ForEachVoice(SetFilterLFOAmount(amount));
  }
};

#endif
