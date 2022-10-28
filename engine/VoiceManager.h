#ifndef __VOICEMANAGER__
#define __VOICEMANAGER__
#pragma once

#include "sys/system.h"

#include "Note.h"
#include "Voice.h"

class VoiceManager {
private:
  static const int number_of_voices_ = 12;

  uint8_t buffer_throttle = 0;
  float midi_[127];
  float key_follow_[127];
  float volume_;
  Voice voices_[number_of_voices_];
  daisysp::Oscillator lfo_;

  Voice *FindFreeVoice(Note *new_note);

public:
  float last_sample;
  uint8_t active_voices;

  VoiceManager() : volume_(1.0f), last_sample(0.f), active_voices(0){};
  ~VoiceManager(){};

  void Init(float sample_rate);
  void Process(float *left, float *right);
  void SetVolume(float new_vol);

#define ForEachVoice(expr)                                                     \
  for (int i = 0; i < number_of_voices_; i++) {                                \
    Voice &voice = voices_[i];                                                 \
    voice.expr;                                                                \
  }

  inline void OnNoteOn(int midi_note, int velocity) {
    Note note = {.freq = midi_[midi_note],
                 .key_follow = key_follow_[midi_note],
                 .midi = midi_note,
                 .velocity = daisysp::fmap((float)velocity, 0.001f, 1.f,
                                           daisysp::Mapping::LOG)};

    Voice *voice = FindFreeVoice(&note);
    if (!voice) {
      return;
    }

    ForEachVoice(IncrementAge());

    voice->SetNote(note);
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
