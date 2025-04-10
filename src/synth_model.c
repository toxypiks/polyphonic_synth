#include "synth_model.h"
#include "oscillator.h"
#include "envelop.h"
#include <stdlib.h>
#include <string.h>

SynthModel* create_synth_model()
{
  SynthModel* synth_model = (SynthModel*)malloc(sizeof(SynthModel));
  memset(synth_model->osc.amp, 1.0f, 1024);
  synth_model->osc.freq = 440.0f;
  synth_model->osc.phase = 0.0f;

  //TODO: init osc and envelop
  return synth_model;
}

// TODO length of tone_buf as parameter
void synth_model_process(SynthModel* synth_model,
                         float* tone_buf,
                         float new_vol,
                         float* adsr_current_value,
                         float* adsr_length)
{
  float sum_ads = 0.0f;
  float sum_adsr = 0.0f;

  // TODO handle volume control
  //change_amp(&synth_model->osc, new_vol);
  gen_signal_in_buf(&synth_model->osc, tone_buf, 1024, &synth_model->adsr_envelop);
  *adsr_current_value = synth_model->adsr_envelop.current_value;
  if ((*adsr_current_value <= 0.0f) && (synth_model->adsr_envelop.envelop_state == DEFAULT)){
      synth_model->osc.is_end = true;
  }

  // calculate adsr x,y0,y1 values
  sum_ads = (synth_model->adsr_envelop.attack + synth_model->adsr_envelop.decay + synth_model->adsr_envelop.sustain_length);

  if ((synth_model->adsr_envelop.sample_count < sum_ads*48000.0f) && (synth_model->adsr_envelop.envelop_state ==  PRESSED_ATTACK
                                                            || synth_model->adsr_envelop.envelop_state == PRESSED_DECAY
                                                            || synth_model->adsr_envelop.envelop_state == PRESSED_SUSTAIN))
  {
    *adsr_length = synth_model->adsr_envelop.sample_count/48000.0f;
  } else if ((synth_model->adsr_envelop.sample_count > sum_adsr*48000.0f) && synth_model->adsr_envelop.envelop_state ==  PRESSED_ATTACK
             || synth_model->adsr_envelop.envelop_state == PRESSED_DECAY
             || synth_model->adsr_envelop.envelop_state == PRESSED_SUSTAIN)
  {
    *adsr_length = sum_ads;
  } else if (synth_model->adsr_envelop.envelop_state == RELEASED) {
      *adsr_length =  synth_model->adsr_envelop.attack
                      + synth_model->adsr_envelop.decay
                      + synth_model->adsr_envelop.sustain_length
                      + (synth_model->adsr_envelop.sample_count_release /(48000.0f));
  }
}

void synth_model_envelope_update(SynthModel* synth_model,
                                 float attack,
                                 float decay,
                                 float sustain,
                                 float release,
                                 bool is_triggered)
{
    // TODO: signal state via ringbuffer
    envelop_change_adsr(&synth_model->adsr_envelop,attack, decay, sustain, release);
    envelop_trigger(&synth_model->adsr_envelop, is_triggered);
}


void synth_model_clear(SynthModel* synth_model)
{
  free(synth_model);
}
