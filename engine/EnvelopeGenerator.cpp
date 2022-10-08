#include "EnvelopeGenerator.h"

float EnvelopeGenerator::sampleRate = 48000.0;

/**
Used to keep track of where the envelope
generator currently is in the attack, decay and release stage.

*/
float EnvelopeGenerator::nextSample() {
  if (currentStage != ENVELOPE_STAGE_OFF &&
      currentStage != ENVELOPE_STAGE_SUSTAIN) {
    if (currentSampleIndex == nextStageSampleIndex) {
      // Gets the next item from EnvelopeStage enum
      // modulo operator it goes back to off or release.
      EnvelopeStage newStage =
          static_cast<EnvelopeStage>((currentStage + 1) % kNumEnvelopeStages);
      // Goes into the next stage
      enterStage(newStage);
    }

    currentLevel *= multiplier;
    currentSampleIndex++;
  }
  return currentLevel;
} /**
  Calculates volume change based on two values and a given time.
  On every sample the current env value is multiplied with this value.

  Takes te startLevel and endLevel and transition's lengthISamples and
  calculates a multiplier slightly below or above 1.
  Mulitply currentLevel with it to get an exponantional transition

  Based on Christian Schoenebeck's Fast Exponential Envelope Generator
  http://www.musicdsp.org/showone.php?id=189
  */
void EnvelopeGenerator::calculateMultiplier(float startLevel, float endLevel,
                                            float lengthInSamples) {

  multiplier = 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
}
/**
Enters a stage defined in the enums in EnvelopeGenerator.h
*/
void EnvelopeGenerator ::enterStage(EnvelopeStage newStage) {
  if (currentStage == newStage)
    return;
  if (currentStage == ENVELOPE_STAGE_OFF) {
    beginEnvelopeCycle();
  }
  if (newStage == ENVELOPE_STAGE_OFF) {
    finishedEnvelopeCycle();
  }
  currentStage = newStage;
  currentSampleIndex = 0;
  if (currentStage == ENVELOPE_STAGE_OFF ||
      currentStage == ENVELOPE_STAGE_SUSTAIN) {
    nextStageSampleIndex = 0;
  } else {
    nextStageSampleIndex = stageValue[currentStage] * sampleRate;
  }
  switch (newStage) {
  case ENVELOPE_STAGE_OFF:
    currentLevel = 0.0;
    multiplier = 1.0;
    break;

  // start from minimum level and calculate the multiplier
  case ENVELOPE_STAGE_ATTACK:
    currentLevel = minimumLevel;
    calculateMultiplier(currentLevel, 1.0, nextStageSampleIndex);
    break;

  // let the level fall to the sustain level fmax to make sure its not 0
  case ENVELOPE_STAGE_DECAY:
    currentLevel = 1.0;
    calculateMultiplier(currentLevel,
                        fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel),
                        nextStageSampleIndex);
    break;

  //  assign the stageValue to the current level
  case ENVELOPE_STAGE_SUSTAIN:
    currentLevel = stageValue[ENVELOPE_STAGE_SUSTAIN];
    multiplier = 1.0;
    break;

  // Decays from current level to the minimum level
  case ENVELOPE_STAGE_RELEASE:
    // could go straight from a/d to r
    // no need to change current level
    calculateMultiplier(currentLevel, minimumLevel, nextStageSampleIndex);
    break;
  default:
    break;
  }
}

/**
Sets the sample rate
*/
void EnvelopeGenerator::setSampleRate(float newSampleRate) {
  sampleRate = newSampleRate;
}

/**
Whenever we change the values of the envelope the generator
should update its values immediately.
*/
void EnvelopeGenerator::setStageValue(EnvelopeStage stage, float value) {
  stageValue[stage] = value;
  if (stage == currentStage) {
    // Re-calculate multiplier and nextStageSampleIndex
    if (currentStage == ENVELOPE_STAGE_ATTACK ||
        currentStage == ENVELOPE_STAGE_DECAY ||
        currentStage == ENVELOPE_STAGE_RELEASE) {
      float nextLevelValue;
      switch (currentStage) {
      case ENVELOPE_STAGE_ATTACK:
        nextLevelValue = 1.0;
        break;

      case ENVELOPE_STAGE_DECAY:
        nextLevelValue = fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel);
        break;
      case ENVELOPE_STAGE_RELEASE:
        nextLevelValue = minimumLevel;
        break;
      default:
        break;
      }
      // How far generator is into current stage
      float currentStageProcess =
          (currentSampleIndex + 0.0) / nextStageSampleIndex;
      // How much of the current stage is left
      float remainingStageProcess = 1.0 - currentStageProcess;
      unsigned long long samplesUntilNextStage =
          remainingStageProcess * value * sampleRate;
      nextStageSampleIndex = currentSampleIndex + samplesUntilNextStage;
      calculateMultiplier(currentLevel, nextLevelValue, samplesUntilNextStage);

    } else if (currentStage == ENVELOPE_STAGE_SUSTAIN) {
      currentLevel = value;
    }
  }
  if (currentStage == ENVELOPE_STAGE_DECAY && stage == ENVELOPE_STAGE_SUSTAIN) {
    // We have to decay to a different sustain value than before
    // and re-calculate multiplier
    float sampleUntilNextStage = nextStageSampleIndex - currentSampleIndex;
    calculateMultiplier(currentLevel,
                        fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel),
                        sampleUntilNextStage);
  }
}
