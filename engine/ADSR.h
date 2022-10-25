//
//  ADRS.h
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
//  2020-04-08  njr   changed double to float; large target ratio and rate
//  resulted in exp returning 1 in calcCoef
//

#ifndef ADRS_h
#define ADRS_h

#include <math.h>

class ADSR {
public:
  ADSR(void);
  ~ADSR(void);
  float Process(void);
  float GetOutput(void);
  int GetState(void);
  void Gate(int on);
  void SetAttackRate(float rate);
  void SetDecayRate(float rate);
  void SetReleaseRate(float rate);
  void SetSustainLevel(float level);
  void SetTargetRatioA(float targetRatio);
  void SetTargetRatioDR(float targetRatio);
  void Reset(void);

  enum envState {
    ENV_IDLE = 0,
    ENV_ATTACK,
    ENV_DECAY,
    ENV_SUSTAIN,
    ENV_RELEASE
  };

protected:
  int state;
  float output;
  float attackRate;
  float decayRate;
  float releaseRate;
  float attackCoef;
  float decayCoef;
  float releaseCoef;
  float sustainLevel;
  float targetRatioA;
  float targetRatioDR;
  float attackBase;
  float decayBase;
  float releaseBase;

  float CalcCoef(float rate, float targetRatio);
};

#endif
