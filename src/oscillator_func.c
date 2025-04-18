#include "oscillator_func.h"
#include <math.h>
#include <stddef.h>

void sine_wave(float freq, float phase, size_t step, float *delta_phase, float *value)
{
   *delta_phase = fmod((2.0f*M_PI*freq*step/48000.0f + phase), 2.0f*M_PI);
   *value = sin(*delta_phase);
}
