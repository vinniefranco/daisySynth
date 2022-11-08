#include "Control.h"

void Control::Init(ControlParam config) {
  c_ceil = config.ceil;
  c_floor = config.floor;
  c_mapping = config.mapping;
  type_of = config.type;
}

void Control::SetValue(uint8_t new_value) {
  float coerced_input = daisysp::fmap(new_value / 127.f, c_floor, c_ceil, c_mapping);

  if (type_of == CONTROL_AVG) {
    value = 0.9f * (coerced_input - value) + value;
  } else {
    value = coerced_input;
  }
}
