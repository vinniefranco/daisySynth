//
//  WaveUtils.cpp
//
//  Test wavetable oscillator
//
//  Created by Nigel Redmon on 2/18/13
//  EarLevel Engineering: earlevel.com
//  Copyright 2013 Nigel Redmon
//
//  For a complete explanation of the wavetable oscillator and code,
//  read the series of articles by the author, starting here:
//  www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own
//  purposes, free or commercial.
//
//  1.01  njr  2016-01-03   changed "> minVal" to "< minVal" to fix optimization
//  of number of tables 1.1   njr  2019-04-30   changed addWaveTable to
//  AddWaveTable to accomodate WaveTableOsc update
//                          added filleTables2, which allows selection of
//                          minimum and maximum frequencies
//  1.2   njr  2020-03-20   fixed memory leak
//

#include "WaveUtils.h"
#include <math.h>

void fft(int N, float *ar, float *ai);
float makeWaveTable(waveTable *table, int len, float *ar, float *ai,
                    float scale, float topFreq, const int totalSlots);

//
// fillTables:
//
// The main function of interest here; call this with a pointer to an new, empty
// oscillator, and the real and imaginary arrays and their length. The function
// fills the oscillator with all wavetables necessary for full-bandwidth
// operation, based on one table per octave, and returns the number of tables.
//
int fillTables(waveTable *table, float *freqWaveRe, float *freqWaveIm,
               int numSamples, int *n_tables, const int totalSlots) {
  int idx;

  // zero DC offset and Nyquist
  freqWaveRe[0] = freqWaveIm[0] = 0.0;
  freqWaveRe[numSamples >> 1] = freqWaveIm[numSamples >> 1] = 0.0;

  // determine maxHarmonic, the highest non-zero harmonic in the wave
  int maxHarmonic = numSamples >> 1;
  const float minVal = 0.000001; // -120 dB
  while ((fabs(freqWaveRe[maxHarmonic]) + fabs(freqWaveIm[maxHarmonic]) <
          minVal) &&
         maxHarmonic)
    --maxHarmonic;

  // calculate topFreq for the initial wavetable
  // maximum non-aliasing playback rate is 1 / (2 * maxHarmonic), but we allow
  // aliasing up to the point where the aliased harmonic would meet the next
  // octave table, which is an additional 1/3
  float topFreq = 2.0 / 3.0 / maxHarmonic;

  // for subsquent tables, float topFreq and remove upper half of harmonics
  float *ar = new float[numSamples];
  float *ai = new float[numSamples];
  float scale = 0.0;
  int numTables = 0;
  while (maxHarmonic) {
    // fill the table in with the needed harmonics
    for (idx = 0; idx < numSamples; idx++)
      ar[idx] = ai[idx] = 0.0;
    for (idx = 1; idx <= maxHarmonic; idx++) {
      ar[idx] = freqWaveRe[idx];
      ai[idx] = freqWaveIm[idx];
      ar[numSamples - idx] = freqWaveRe[numSamples - idx];
      ai[numSamples - idx] = freqWaveIm[numSamples - idx];
    }

    // make the wavetable
    scale = makeWaveTable(table, numSamples, ar, ai, scale, topFreq, n_tables,
                          totalSlots);
    numTables++;

    // prepare for next table
    topFreq *= 2;
    maxHarmonic >>= 1;
  }
  delete[] ar;
  delete[] ai;
  return numTables;
}

float makeWaveTable(waveTable *table, int len, float *ar, float *ai,
                    float scale, float topFreq, int *n_tables,
                    const int totalSlots) {
  fft(len, ar, ai);

  if (scale == 0.0) {
    // calc normal
    float max = 0;
    for (int idx = 0; idx < len; idx++) {
      float temp = fabs(ai[idx]);
      if (max < temp)
        max = temp;
    }
    scale = 1.0 / max * .999;
  }

  // normalize
  float *wave = new float[len];
  for (int idx = 0; idx < len; idx++)
    wave[idx] = ai[idx] * scale;

  if (AddWaveTable(len, table, n_tables, totalSlots, wave, topFreq))
    scale = 0.0;

  delete[] wave;
  return scale;
}

int AddWaveTable(int len, waveTable *table, int *n_tables, const int totalSlots,
                 float *waveTableIn, float topFreq) {

  if (*n_tables < totalSlots) {
    float *waveTable = table[*n_tables].waveTable = new float[len + 1];
    table[*n_tables].waveTableLen = len;
    table[*n_tables].topFreq = topFreq;
    ++*n_tables;

    // fill in wave
    for (long idx = 0; idx < len; idx++)
      waveTable[idx] = waveTableIn[idx];

    waveTable[len] = waveTable[0]; // duplicate for interpolation wraparound

    return 0;
  }
  return *n_tables;
}

void sawOsc(waveTable *table, int *n_tables, const int totalSlots) {
  int tableLen = 2048; // to give full bandwidth from 20 Hz
  int idx;
  float *freqWaveRe = new float[tableLen];
  float *freqWaveIm = new float[tableLen];

  // make a sawtooth
  for (idx = 0; idx < tableLen; idx++) {
    freqWaveIm[idx] = 0.0;
  }
  freqWaveRe[0] = freqWaveRe[tableLen >> 1] = 0.0;
  for (idx = 1; idx < (tableLen >> 1); idx++) {
    freqWaveRe[idx] = 1.0 / idx;                   // sawtooth spectrum
    freqWaveRe[tableLen - idx] = -freqWaveRe[idx]; // mirror
  }

  fillTables(table, freqWaveRe, freqWaveIm, tableLen, n_tables, totalSlots);

  delete[] freqWaveRe;
  delete[] freqWaveIm;
}

//
// fft
//
// I grabbed (and slightly modified) this Rabiner & Gold translation...
//
// (could modify for real data, could use a template version, blah blah--just
// keeping it short)
//
void fft(int N, float *ar, float *ai)
/*
 in-place complex fft

 After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)

 program adapted from FORTRAN
 by K. Steiglitz  (ken@princeton.edu)
 Computer Science Dept.
 Princeton University 08544          */
{
  int i, j, k, L;           /* indexes */
  int M, TEMP, LE, LE1, ip; /* M = log N */
  int NV2, NM1;
  float t; /* temp */
  float Ur, Ui, Wr, Wi, Tr, Ti;
  float Ur_old;

  // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2

  NV2 = N >> 1;
  NM1 = N - 1;
  TEMP = N; /* get M = log N */
  M = 0;
  while (TEMP >>= 1)
    ++M;

  /* shuffle */
  j = 1;
  for (i = 1; i <= NM1; i++) {
    if (i < j) { /* swap a[i] and a[j] */
      t = ar[j - 1];
      ar[j - 1] = ar[i - 1];
      ar[i - 1] = t;
      t = ai[j - 1];
      ai[j - 1] = ai[i - 1];
      ai[i - 1] = t;
    }

    k = NV2; /* bit-reversed counter */
    while (k < j) {
      j -= k;
      k /= 2;
    }

    j += k;
  }

  LE = 1.;
  for (L = 1; L <= M; L++) { // stage L
    LE1 = LE;                // (LE1 = LE/2)
    LE *= 2;                 // (LE = 2^L)
    Ur = 1.0;
    Ui = 0.;
    Wr = cos(M_PI / (float)LE1);
    Wi = -sin(M_PI / (float)LE1); // Cooley, Lewis, and Welch have "+" here
    for (j = 1; j <= LE1; j++) {
      for (i = j; i <= N; i += LE) { // butterfly
        ip = i + LE1;
        Tr = ar[ip - 1] * Ur - ai[ip - 1] * Ui;
        Ti = ar[ip - 1] * Ui + ai[ip - 1] * Ur;
        ar[ip - 1] = ar[i - 1] - Tr;
        ai[ip - 1] = ai[i - 1] - Ti;
        ar[i - 1] = ar[i - 1] + Tr;
        ai[i - 1] = ai[i - 1] + Ti;
      }
      Ur_old = Ur;
      Ur = Ur_old * Wr - Ui * Wi;
      Ui = Ur_old * Wi + Ui * Wr;
    }
  }
}
