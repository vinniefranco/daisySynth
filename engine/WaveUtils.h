//
//  WaveUtils.h
//
//  Created by Nigel Redmon on 2/18/13
//  Modified for use on daisy seed by Vincent Franco
//
//

#ifndef WaveUtils_h
#define WaveUtils_h

struct waveTable {
  float topFreq;
  int waveTableLen;
  float *waveTable;
};

struct WaveSlot {
  waveTable *wt;
  int wt_slots;
};

int AddWaveTable(int len, waveTable *table, int *n_tables, const int totalSlots,
                 float *waveTableIn, float topFreq);
int fillTables(waveTable *table, float *freqWaveRe, float *freqWaveIm,
               int numSamples, int *n_tables, const int totalslots);
float makeWaveTable(waveTable *table, int len, float *ar, float *ai,
                    float scale, float topFreq, int *n_tables,
                    const int totalslots);

void sawOsc(waveTable *table, int *n_tables, const int totalslots);
void sqrOsc(waveTable *table, int *n_tables, const int totalslots);

#endif
