#ifdef VF_SYNTH_POT
#define VF_SYNTH_POT

namespace daisy {

class SynthPot {
public:
  void Init(daisy::DaisySeed *seed, uint32_t *ticker, uint8_t newchannel,
            float newmin, float newmax) {
    channel = newchannel;
    hw = seed;
    max = newmax;
    min = newmin;
    tick = ticker;
    lmin = logf(newmin < 0.0000001f ? 0.0000001f : newmin);
    lmax = logf(newmax);
  }

  float GetFreq() { return expf((value * (lmax - lmin)) + lmin); }

  void Tick() {
    if (*tick % 100000 == 0) {
      float input = hw->adc.GetFloat(0);
      value = 0.9f * (input - value) + value;
    }
  }

  SynthPot();
  ~SynthPot();

private:
  daisy::DaisySeed *hw;
  uint32_t *tick;
  uint8_t channel;
  float min, max, lmin, lmax;
  float value;
};

} // namespace daisy

#endif // VF_SYNTH_POT
