// By Paul Kellett
// http://www.musicdsp.org/showone.php?id=29
#include "Filter.h"

/**
calculates a lowpass, highpass and bandpass filter dependng on the mode selected
*/
float Filter::process(float inputValue) {
  if (inputValue == 0.0)
    return inputValue;
  float calculatedCutoff = getCalculatedCutoff();
  //
  buf0 +=
      calculatedCutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
  buf1 += calculatedCutoff * (buf0 - buf1);
  buf2 += calculatedCutoff * (buf1 - buf2);
  buf3 += calculatedCutoff * (buf2 - buf3);
  switch (mode) {
  case FILTER_MODE_LOWPASS:
    return buf3;
  default:
    return 0.0;
  }
}
