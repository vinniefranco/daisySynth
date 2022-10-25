//
//  ADSR.cpp
//
//  Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own
//  purposes, free or commercial.
//
//  1.01  2016-01-02  njr   added calcCoef to SetTargetRatio functions that were
//  in the ADSR widget but missing in this code 1.02  2017-01-04  njr   in
//  calcCoef, checked for rate 0, to support non-IEEE compliant compilers 1.03
//  2020-04-08  njr   changed float to float; large target ratio and rate
//  resulted in exp returning 1 in calcCoef
//

#include "ADSR.h"
#include <math.h>

void ADSR::Init(float new_sample_rate) { sample_rate = new_sample_rate; }

void ADSR::SetAttackRate(float rate) {
  attackRate = rate * sample_rate;
  attackCoef = CalcCoef(attackRate, targetRatioA);
  attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void ADSR::SetDecayRate(float rate) {
  decayRate = rate * sample_rate;
  decayCoef = CalcCoef(decayRate, targetRatioDR);
  decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void ADSR::SetReleaseRate(float rate) {
  releaseRate = rate * sample_rate;
  releaseCoef = CalcCoef(releaseRate, targetRatioDR);
  releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

void ADSR::SetKillRate(float rate) {
  killRate = rate * sample_rate;
  killCoef = CalcCoef(killRate, targetRatioDR);
  killBase = -targetRatioDR * (1.0 - releaseCoef);
}

float ADSR::CalcCoef(float rate, float targetRatio) {
  return (rate <= 0) ? 0.0
                     : exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}

void ADSR::SetSustainLevel(float level) {
  sustainLevel = level;
  decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void ADSR::SetTargetRatioA(float targetRatio) {
  if (targetRatio < 0.000000001)
    targetRatio = 0.000000001; // -180 dB
  targetRatioA = targetRatio;
  attackCoef = CalcCoef(attackRate, targetRatioA);
  attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void ADSR::SetTargetRatioDR(float targetRatio) {
  if (targetRatio < 0.000000001)
    targetRatio = 0.000000001; // -180 dB
  targetRatioDR = targetRatio;
  decayCoef = CalcCoef(decayRate, targetRatioDR);
  releaseCoef = CalcCoef(releaseRate, targetRatioDR);
  decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
  releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

float ADSR::Process(bool kill_tail) {
  if (kill_tail) {
    output = killBase + output * killCoef;
    if (output <= 0.0) {
      output = 0.0;
      state = ENV_IDLE;
    }

    return output;
  }

  switch (state) {
  case ENV_IDLE:
    break;
  case ENV_ATTACK:
    output = attackBase + output * attackCoef;
    if (output >= 1.0) {
      output = 1.0;
      state = ENV_DECAY;
    }
    break;
  case ENV_DECAY:
    output = decayBase + output * decayCoef;
    if (output <= sustainLevel) {
      output = sustainLevel;
      state = ENV_SUSTAIN;
    }
    break;
  case ENV_SUSTAIN:
    break;
  case ENV_RELEASE:
    output = releaseBase + output * releaseCoef;
    if (output <= 0.0) {
      output = 0.0;
      state = ENV_IDLE;
    }
  }
  return output;
}

void ADSR::Gate(int gate) {
  if (gate) {
    state = ENV_ATTACK;
  } else if (state != ENV_IDLE) {
    state = ENV_RELEASE;
  }
}

void ADSR::Kill() { state = ENV_KILL; }

int ADSR::GetState() { return state; }

void ADSR::Reset() {
  state = ENV_IDLE;
  output = 0.0;
}

float ADSR::GetOutput() { return output; }
