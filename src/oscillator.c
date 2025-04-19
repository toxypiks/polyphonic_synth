#include "oscillator.h"
#include <math.h>
#include "envelop.h"
#include "oscillator_func.h"

void set_oscillator(Oscillator *osc,
                    float freq,
                    float phase,
                    float vel,
                    bool is_on,
                    bool is_end,
                    WAVE_FCT wave_fct)
{

    osc->freq   = freq;
    osc->phase  = phase;
    osc->vel    = vel;
    osc->is_on  = is_on;
    osc->is_end = is_end;
    osc->func = wave_fct_factory(wave_fct);
}

void change_frequency(Oscillator* osc, float new_freq)
{
  osc->freq = new_freq;
}

void change_amp(Oscillator* osc, float new_amp)
{
  float old_amp = osc->amp[1023];
  float delta_step = old_amp == new_amp ? 0 : (new_amp - old_amp)/1024.0f;
  for (size_t i = 0; i < 1024; ++i) {
    osc->amp[i] = old_amp + (i*delta_step);
  }
}

void gen_signal_in_buf(Oscillator* osc, float* buf, size_t buf_length)
{
  float new_phase = 0.0f;
  for(size_t i = 0; i < buf_length; ++i) {
    float phase = 0.0f;
    float value = 0.0f;
    // sine_wave(osc->freq, osc->phase, i, &phase, &value);
    osc->func(osc->freq, osc->phase, i, &phase, &value);
    buf[i] = value;
    new_phase = phase;
  }
  osc->phase = new_phase;

  // check if phase is NaN
  if (osc->phase != osc->phase) {
    osc->phase = 0.0;
  }
}
