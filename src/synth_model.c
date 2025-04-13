#include "synth_model.h"
#include "oscillator.h"
#include "envelop.h"
#include <stdlib.h>
#include <string.h>

SynthModel* create_synth_model()
{
  SynthModel* synth_model = (SynthModel*)malloc(sizeof(SynthModel));
  memset(synth_model->osc.amp, 1.0f, 1024);
  set_oscillator(&synth_model->osc, 440.0f, 0.0f, 0.0f, false, false);
  set_evelope(&synth_model->adsr_envelop, 0.05f, 0.15f, 0.5f, 0.3f);
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
  size_t buf_length = 1024;
  gen_signal_in_buf(&synth_model->osc, tone_buf, buf_length);
  envelop_apply_in_buf(&synth_model->adsr_envelop, tone_buf, buf_length);

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

void synth_model_clear(SynthModel* synth_model)
{
  free(synth_model);
}
