#ifndef __EnvelopeGenerator__
#define __EnvelopeGenerator__
#pragma once
#include "GallantSignal.h"
#include "daisysp.h"

class EnvelopeGenerator {
public:
  enum EnvelopeStage {
    ENVELOPE_STAGE_OFF = 0,
    ENVELOPE_STAGE_ATTACK,
    ENVELOPE_STAGE_DECAY,
    ENVELOPE_STAGE_SUSTAIN,
    ENVELOPE_STAGE_RELEASE,
    kNumEnvelopeStages
  };
  void enterStage(EnvelopeStage newStage);
  float nextSample();
  static void setSampleRate(float newSampleRate);
  inline EnvelopeStage getCurrentStage() const { return currentStage; };
  const float minimumLevel;
  void setStageValue(EnvelopeStage stage, float value);
  Gallant::Signal0<> beginEnvelopeCycle;
  Gallant::Signal0<> finishedEnvelopeCycle;
  void reset() {
    currentStage = ENVELOPE_STAGE_OFF;
    currentLevel = minimumLevel;
    multiplier = 1.0;
    currentSampleIndex = 0;
    nextStageSampleIndex = 0;
  }

  EnvelopeGenerator()
      : minimumLevel(0.0001), currentStage(ENVELOPE_STAGE_OFF),
        currentLevel(minimumLevel), multiplier(1.0), currentSampleIndex(0),
        nextStageSampleIndex(0) {
    stageValue[ENVELOPE_STAGE_OFF] = 0.0;
    stageValue[ENVELOPE_STAGE_ATTACK] = 0.04; // 0.01
    stageValue[ENVELOPE_STAGE_DECAY] = 1.2;   // 0.5
    stageValue[ENVELOPE_STAGE_SUSTAIN] = 0.8;
    stageValue[ENVELOPE_STAGE_RELEASE] = 0.5; // 1.0
  };

private:
  EnvelopeStage currentStage;
  float currentLevel;
  float multiplier;
  static float sampleRate;
  float stageValue[kNumEnvelopeStages];
  void calculateMultiplier(float startLevel, float endLevel,
                           float lengthInSamples);
  unsigned long long currentSampleIndex;
  unsigned long long nextStageSampleIndex;
};

#endif // !__EnvelopeGenerator__
