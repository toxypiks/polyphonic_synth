#include "oscillator_func.h"
#include <math.h>
#include <stddef.h>

void (*wave_fct_factory(enum WAVE_FCT which_fct))(float, float, size_t, float*, float*)
{
    void (*wave_fct[4])(float, float, size_t, float*, float*) = {
        sine_wave,
        saw_wave,
        triangle_wave,
        square_wave
    };
    return wave_fct[which_fct];
}

void sine_wave(float freq, float phase, size_t step, float *delta_phase, float *value)
{
   *delta_phase = fmod((2.0f*M_PI*freq*step/48000.0f + phase), 2.0f*M_PI);
   *value = sin(*delta_phase);
}

void saw_wave(float freq, float phase, size_t step, float *delta_phase, float *value)
{
   *delta_phase = fmod((2.0f*M_PI*freq*step/48000.0f + phase), 2.0f*M_PI);
   *value = sin(*delta_phase);
}

void triangle_wave(float freq, float phase, size_t step, float *delta_phase, float *value)
{
   *delta_phase = fmod((2.0f*M_PI*freq*step/48000.0f + phase), 2.0f*M_PI);
   *value = sin(*delta_phase);
}

void square_wave(float freq, float phase, size_t step, float *delta_phase, float *value)
{
   *delta_phase = fmod((2.0f*M_PI*freq*step/48000.0f + phase), 2.0f*M_PI);
   *value = sin(*delta_phase);
}
