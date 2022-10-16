//
//  WaveTableOsc.h
//
//  Created by Nigel Redmon on 2018-10-05
//  EarLevel Engineering: earlevel.com
//  Copyright 2018 Nigel Redmon
//
//  For a complete explanation of the wavetable oscillator and code,
//  read the series of articles by the author, starting here:
//  www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
//
//  This version has optimizations described here:
//  www.earlevel.com/main/2019/04/28/wavetableosc-optimized/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own
//  purposes, free or commercial.
//

#ifndef WaveTableOsc_h
#define WaveTableOsc_h

#include "daisy_seed.h"

#include "WaveUtils.h"

class WaveTableOsc {
public:
  int current_wt = 0; // current table, based on current frequency

  WaveTableOsc(void) {}
  ~WaveTableOsc(void) {}

  void Init(float new_sample_rate) { sample_rate_ = new_sample_rate; }

  void SetWavetable(waveTable *new_wt, int total_slots) {
    wt = new_wt;
    wt_slots = total_slots;
  }

  //
  // SetFrequency: Set normalized frequency, typically 0-0.5 (must be positive
  // and less than 1!)
  //
  void SetFrequency(float inc) {
    m_phase_inc_ = inc;

    // update the current wave table selector
    int wt_selector = 0;
    while ((m_phase_inc_ >= wt[wt_selector].topFreq) &&
           (current_wt < (wt_slots - 1))) {
      ++wt_selector;
    }
    current_wt = wt_selector;
  }

  void ResetPhasor() { m_phasor_ = 0.0f; }

  void SetFreq(float new_freq) { SetFrequency(new_freq / sample_rate_); }

  //
  // SetPhaseOffset: Phase offset for PWM, 0-1
  //
  void SetPhaseOffset(float offset) { m_phase_offset_ = offset; }

  //
  // UpdatePhase: Call once per sample
  //
  void UpdatePhase(void) {
    m_phasor_ += m_phase_inc_;

    if (m_phasor_ >= 1.0)
      m_phasor_ -= 1.0;
  }

  //
  // Process: Update phase and get output
  //
  float Process(void) {
    UpdatePhase();
    return GetOutput();
  }

  //
  // GetOutput: Returns the current oscillator output
  //
  float GetOutput(void) {
    waveTable *waveTable = &wt[current_wt];

    // linear interpolation
    float temp = m_phasor_ * waveTable->waveTableLen;
    int intPart = temp;
    float fracPart = temp - intPart;
    float samp0 = waveTable->waveTable[intPart];
    float samp1 = waveTable->waveTable[intPart + 1];
    return samp0 + (samp1 - samp0) * fracPart;
  }

  //
  // getOutputMinusOffset
  //
  // for variable pulse width: initialize to sawtooth,
  // set phaseOfs to duty cycle, use this for osc output
  //
  // returns the current oscillator output
  //
  float GetOutputMinusOffset() {
    waveTable *waveTable = &wt[current_wt];
    int len = waveTable->waveTableLen;
    float *wave = waveTable->waveTable;

    // linear
    float temp = m_phasor_ * len;
    int intPart = temp;
    float fracPart = temp - intPart;
    float samp0 = wave[intPart];
    float samp1 = wave[intPart + 1];
    float samp = samp0 + (samp1 - samp0) * fracPart;

    // and linear again for the offset part
    float offsetPhasor = m_phasor_ + m_phase_offset_;
    if (offsetPhasor > 1.0)
      offsetPhasor -= 1.0;
    temp = offsetPhasor * len;
    intPart = temp;
    fracPart = temp - intPart;
    samp0 = wave[intPart];
    samp1 = wave[intPart + 1];
    return samp - (samp0 + (samp1 - samp0) * fracPart);
  }

protected:
  float m_phasor_ = 0.0f;       // phase accumulator
  float m_phase_inc_ = 0.0f;    // phase increment
  float m_phase_offset_ = 0.5f; // phase offset for PWM
  float sample_rate_;

  waveTable *wt;
  int wt_slots = 0;
};

#endif
