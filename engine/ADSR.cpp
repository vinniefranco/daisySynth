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

ADSR::ADSR(void) {
  Reset();
  SetAttackRate(0);
  SetDecayRate(0);
  SetReleaseRate(0);
  SetSustainLevel(1.0);
  SetTargetRatioA(0.3);
  SetTargetRatioDR(0.0001);
}

ADSR::~ADSR(void) {}

void ADSR::SetAttackRate(float rate) {
  attackRate = rate;
  attackCoef = CalcCoef(rate, targetRatioA);
  attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void ADSR::SetDecayRate(float rate) {
  decayRate = rate;
  decayCoef = CalcCoef(rate, targetRatioDR);
  decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
}

void ADSR::SetReleaseRate(float rate) {
  releaseRate = rate;
  releaseCoef = CalcCoef(rate, targetRatioDR);
  releaseBase = -targetRatioDR * (1.0 - releaseCoef);
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
