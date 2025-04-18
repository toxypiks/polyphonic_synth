#ifndef OSCILLATOR_FUNC_H_
#define OSCILLATOR_FUNC_H_

#include <stddef.h>

typedef enum WAVE_FCT {
    SINE_WAVE = 0,
    SAW_WAVE = 1,
    TRIANGLE_WAVE = 2,
    SQUARE_WAVE = 3
} WAVE_FCT;

void (*wave_fct_factory(WAVE_FCT which_fct))(float, float, size_t, float*, float*);

void sine_wave(float freq, float phase, size_t step, float *delta_phase, float *value);
void saw_wave(float freq, float phase, size_t step, float *delta_phase, float *value);
void triangle_wave(float freq, float phase, size_t step, float *delta_phase, float *value);
void square_wave(float freq, float phase, size_t step, float *delta_phase, float *value);

#endif // OSCILLATOR_FUNC_H_
