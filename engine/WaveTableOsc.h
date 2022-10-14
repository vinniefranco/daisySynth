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

#include "WaveUtils.h"

#include "daisy_seed.h"

static constexpr int numWaveTableSlots =
    18; // simplify allocation with reasonable maximum
static waveTable DSY_SDRAM_BSS saw_slot[numWaveTableSlots];
static int saw_wt_count;

class WaveTableOsc {
public:
  WaveTableOsc(void) { sawOsc(saw_slot, &saw_wt_count, numWaveTableSlots); }
  ~WaveTableOsc(void) {
    // for (int idx = 0; idx < numWaveTableSlots; idx++) {
    //   float *temp = WaveTableOsc::mWaveTables[idx].waveTable;
    //   if (temp != 0)
    //     delete[] temp;
    // }
  }

  //
  // SetFrequency: Set normalized frequency, typically 0-0.5 (must be positive
  // and less than 1!)
  //
  void SetFrequency(float inc) {
    mPhaseInc = inc;

    // update the current wave table selector
    int curWaveTable = 0;
    while ((mPhaseInc >= saw_slot[curWaveTable].topFreq) &&
           (curWaveTable < (saw_wt_count - 1))) {
      ++curWaveTable;
    }
    mCurWaveTable = curWaveTable;
  }

  void SetFreq(float inc) {
    mPhaseInc = inc / 48000.0f;

    // update the current wave table selector
    int curWaveTable = 0;
    while ((mPhaseInc >= saw_slot[curWaveTable].topFreq) &&
           (curWaveTable < (saw_wt_count - 1))) {
      ++curWaveTable;
    }
    mCurWaveTable = curWaveTable;
  }

  //
  // SetPhaseOffset: Phase offset for PWM, 0-1
  //
  void SetPhaseOffset(float offset) { mPhaseOfs = offset; }

  //
  // UpdatePhase: Call once per sample
  //
  void UpdatePhase(void) {
    mPhasor += mPhaseInc;

    if (mPhasor >= 1.0)
      mPhasor -= 1.0;
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
    waveTable waveTable = saw_slot[mCurWaveTable];

    // linear interpolation
    float temp = mPhasor * waveTable.waveTableLen;
    int intPart = temp;
    float fracPart = temp - intPart;
    float samp0 = waveTable.waveTable[intPart];
    float samp1 = waveTable.waveTable[intPart + 1];
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
    waveTable waveTable = saw_slot[mCurWaveTable];
    int len = waveTable.waveTableLen;
    float *wave = waveTable.waveTable;

    // linear
    float temp = mPhasor * len;
    int intPart = temp;
    float fracPart = temp - intPart;
    float samp0 = wave[intPart];
    float samp1 = wave[intPart + 1];
    float samp = samp0 + (samp1 - samp0) * fracPart;

    // and linear again for the offset part
    float offsetPhasor = mPhasor + mPhaseOfs;
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
  float mPhasor = 0.0;   // phase accumulator
  float mPhaseInc = 0.0; // phase increment
  float mPhaseOfs = 0.5; // phase offset for PWM

  // array of wavetables
  int mCurWaveTable = 0; // current table, based on current frequency
};

#endif
