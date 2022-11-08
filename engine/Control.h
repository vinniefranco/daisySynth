#ifndef __CONTROL__
#pragma once

#include "Utility/dsp.h"

struct ControlParam {
  float ceil;
  float floor;
  daisysp::Mapping mapping;
  int type;
};

class Control {
private:
  float value;
  float c_floor;
  float c_ceil;
  daisysp::Mapping c_mapping;
  uint8_t type_of;

public:
  enum {
    CONTROL_AVG = 0,
    CONTROL_DIRECT
  } control_type;

  Control();
  ~Control();

  void Init(ControlParam config);

  void SetValue(uint8_t new_value);
};

#endif // !__CONTROL__
