//
//  WaveUtils.h
//
//  Created by Nigel Redmon on 2/18/13
//
//

#ifndef WaveUtils_h
#define WaveUtils_h

#include "WaveTableOsc.h"

int fillTables(WaveTableOsc *osc, float *freqWaveRe, float *freqWaveIm,
               int numSamples);

int fillTables(float *slot, float *freqWaveRe, float *freqWaveIm,
               int numSamples);
int fillTables2(WaveTableOsc *osc, float *freqWaveRe, float *freqWaveIm,
                int numSamples, float minTop = 0.4, float maxTop = 0);
float makeWaveTable(WaveTableOsc *osc, int len, float *ar, float *ai,
                    float scale, float topFreq);

float makeWaveTable(float *slot, int len, float *ar, float *ai, float scale,
                    float topFreq);

// examples
WaveTableOsc *sawOsc(void);
void sawOsc(float *slot);
WaveTableOsc *waveOsc(float *waveSamples, int tableLen);

#endif
