#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include <stddef.h>
#include "envelop.h"

typedef struct Oscillator {
  float amp[1024];
  float freq;
  float phase;
  float vel;
  bool is_on;
} Oscillator;

void change_frequency(Oscillator* osc, float new_freq);
void change_amp(Oscillator* osc, float new_amp);
void gen_signal_in_buf(Oscillator* osc, float* sum_buf, float* prod_buf, size_t buf_length, Envelop* adsr_envelop);

#endif // OSCILLATOR_H_
