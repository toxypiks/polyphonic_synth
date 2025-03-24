#ifndef SYNTH_MODEL_H
#define SYNTH_MODEL_H

#include "oscillator.h"
#include "envelop.h"

typedef struct SynthModel{
  Oscillator osc;
  Envelop adsr_envelop;
} SynthModel;

SynthModel* create_synth_model();

void synth_model_process(SynthModel* synth_model,
                         float* tone_buf,
                         float new_vol,
                         float* adsr_current_value,
                         float* adsr_length
  );

void synth_model_envelope_update(SynthModel* synth_model,
                                 float attack,
                                 float decay,
                                 float sustain,
                                 float release,
                                 bool is_triggered
  );
void synth_model_clear(SynthModel* synth_model);
#endif // SYNTH_MODEL_H
