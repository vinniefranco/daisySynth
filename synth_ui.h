// #ifdef VF_UI
// #define VF_UI

#include "table_encoder.h"

#include "daisy_core.h"
#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"
#include "per/gpio.h"

namespace daisy {

using SynthOledDisplay = OledDisplay<SSD130x4WireSpi128x64Driver>;

class SynthUI {
public:
  SynthUI() {}
  ~SynthUI() {}

  void Init(DaisySeed *seed, CpuLoadMeter *meter, daisy::Pin clk,
            daisy::Pin data, daisy::Pin swth) {
    hw = seed;
    load_meter = meter;
    // Config Display
    disp_cfg.driver_config.transport_config.pin_config.dc = hw->GetPin(11);
    disp_cfg.driver_config.transport_config.pin_config.reset = hw->GetPin(13);
    display.Init(disp_cfg);

    e.Init(clk, data, swth);

    System::Delay(1000);
  }

  int8_t Read() { return e.Read(); }

  void tick() {
    // static float x = 0.0f;

    // hw->DelayMs(2);

    // switch (Read()) {
    // case 1:
    //   x = x + 0.1f;
    //   break;

    // case -1:
    //   x = x - 0.1f;
    //   break;
    // default:
    //   break;
    // }

    // sprintf(my_output, "Max:" FLT_FMT3, FLT_VAR3(x));
    // hw->PrintLine(my_output);

    const float avg_load = load_meter->GetAvgCpuLoad();
    const float max_load = load_meter->GetMaxCpuLoad();
    const float min_load = load_meter->GetMinCpuLoad();

    System::Delay(100);

    display.Fill(false);

    display.SetCursor(0, 0);
    display.WriteString("CPU----------------", Font_7x10, true);

    sprintf(strbuff1, "Max:" FLT_FMT3, FLT_VAR3(max_load * 100.0f));
    display.SetCursor(0, 14);
    display.WriteString(strbuff1, Font_7x10, true);

    sprintf(strbuff2, "Avg:" FLT_FMT3, FLT_VAR3(avg_load * 100.0f));
    display.SetCursor(0, 28);
    display.WriteString(strbuff2, Font_7x10, true);

    sprintf(strbuff3, "Min:" FLT_FMT3, FLT_VAR3(min_load * 100.0f));
    display.SetCursor(0, 42);
    display.WriteString(strbuff3, Font_7x10, true);

    display.Update();
  }

private:
  char my_output[128];
  TableEncoder e;
  DaisySeed *hw;
  SynthOledDisplay::Config disp_cfg;
  SynthOledDisplay display;
  CpuLoadMeter *load_meter;

  char strbuff1[128];
  char strbuff2[128];
  char strbuff3[128];
};

} // namespace daisy
// #endif // VF_UI
