
#include "daisy_core.h"
#include "per/gpio.h"
#include "sys/system.h"

class TableEncoder {
public:
  enum ActionType {
    NOTHING = 0,
    SINGLE_PRESS,
    LONG_PRESS,
    DOUBLE_PRESS,
    TRIPLE_PRESS,
    INCREASE,
    DECREASE
  };

  void Init(daisy::Pin clk, daisy::Pin data, daisy::Pin swth) {

    store = 0;
    prev_next_code = 0;

    clock_pin.Init(clk, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLUP);
    data_pin.Init(data, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLUP);
    switch_pin.Init(swth, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLUP);
  }
  int8_t Read() {

    static int8_t rot_enc_table[] = {0, 1, 1, 0, 1, 0, 0, 1,
                                     1, 0, 0, 1, 0, 1, 1, 0};
    prev_next_code <<= 2;
    if (data_pin.Read())
      prev_next_code |= 0x02;

    if (clock_pin.Read())
      prev_next_code |= 0x01;

    prev_next_code &= 0x0f;

    if (rot_enc_table[prev_next_code]) {
      store <<= 4;
      store |= prev_next_code;

      if ((store & 0xff) == 0x2b)
        return -1;

      if ((store & 0xff) == 0x17)
        return 1;
    }

    return 0;
  }

  TableEncoder() {}
  ~TableEncoder() {}

private:
  daisy::GPIO clock_pin;
  daisy::GPIO data_pin;
  daisy::GPIO switch_pin;

  uint8_t prev_next_code;
  uint16_t store;
};
