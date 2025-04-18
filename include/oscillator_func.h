#ifndef OSCILLATOR_FUNC_H_
#define OSCILLATOR_FUNC_H_

#include <stddef.h>

void sine_wave(float freq, float phase, size_t step, float *delta_phase, float *value);

#endif // OSCILLATOR_FUNC_H_
