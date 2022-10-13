#include "Engine.h"

void Engine::Process(float *left, float *right) {
  voice_manager.Process(left, right);

  writeToUIBuffer(voice_manager.last_sample);
}

void Engine::writeToUIBuffer(float sample) {
  if (data_cursor >= 128) {
    data_cursor = 0;
    for (size_t i = 0; i < 128; i++) {
      screen_buffer[i] = data_buffer[i];
    }

  } else {
    data_buffer[data_cursor] = sample;
    data_cursor++;
  }
}
