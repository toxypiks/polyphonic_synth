#include <stdio.h>
#include <jack/ringbuffer.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "synth_model.h"
#include "thread_stuff.h"
#include "adsr.h"
#include "msg_handler.h"
#include "midi_msg.h"
#include <math.h>
#include "tone_handler.h"
#include "adsr_display_msg.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

void set_adsr_values(void* adsr_new_raw, void* adsr_values_raw)
{
    ADSR* adsr_values = (ADSR*)adsr_values_raw;
    ADSR* adsr_new = (ADSR*)adsr_new_raw;
    adsr_values->attack  = adsr_new->attack;
    adsr_values->decay   = adsr_new->decay;
    adsr_values->sustain = adsr_new->sustain;
    adsr_values->release = adsr_new->release;
    //free(adsr_new);
};

void* model_gen_signal_thread_fct(void* thread_stuff_raw)
{
    ThreadStuff* thread_stuff = (ThreadStuff*)thread_stuff_raw;
    SynthModel* synth_model = create_synth_model();

    ADSR adsr_values = {0};
    float vol = 0.0;
    float freq = 0.0;
    bool is_play_pressed = false;

    MidiMsg midi_msg = {0};
    ToneHandler tone_handler = { .tone_map = NULL };

    MsgHdl msg_hdl = {0};

    msg_hdl_add_key2fct(&msg_hdl, "adsr", set_adsr_wrapper, (void*)&tone_handler);
    msg_hdl_add_key2fct(&msg_hdl, "vol", set_float_value, (void*)&vol);
    msg_hdl_add_key2fct(&msg_hdl, "midi_msg", set_tone_wrapper, (void*)&tone_handler);

    while(thread_stuff->is_running) {
        msg_hdling(&msg_hdl, &thread_stuff->model_msg_queue);
        msg_hdling(&msg_hdl, &thread_stuff->jack_stuff->midi_msg_queue);

        size_t num_bytes = jack_ringbuffer_read_space(thread_stuff->jack_stuff->ringbuffer_audio);
        float signal_buf[1024];

        tone_handler_retrigger(&tone_handler);
        if (num_bytes < 4800 * sizeof(float)) {
            float tone_buf[1024];
            int synth_model_length = tone_handler_len(&tone_handler);

            if (synth_model_length > 0) {
                for (size_t i = 0; i < synth_model_length; ++i) {
                    ADSRDisplayMsg adsr_display_msg = {0};
                    memset(tone_buf, 0, sizeof(1024));
                    synth_model_process(&tone_handler.tone_map[i].value,
                                        tone_buf,
                                        vol,
                                        &adsr_display_msg.adsr_height,
                                        &adsr_display_msg.adsr_length);
                    adsr_display_msg.key = tone_handler.tone_map[i].key;
                    if (i == 0) {
                        for (size_t j = 0; j < 1024; ++j) {
                            signal_buf[j] = tone_buf[j];
                        }
                    } else {
                        for (size_t j = 0; j < 1024; ++j) {
                            // TODO: Better MIX
                            // bad formula= A + B - A*B
                            signal_buf[j] = (tone_buf[j] + signal_buf[j])*0.5f;
                        }
                    }
                    int ret_adsr = lf_queue_push(&thread_stuff->raylib_msg_queue, "adsr_display_msg", (void*)&adsr_display_msg, sizeof(ADSRDisplayMsg));

                }
            } else {
                memset(signal_buf, 0, 1024*sizeof(float));
            }

            tone_handler_cleanup(&tone_handler);
            // TODO Volume on databuf

            // TODO msg send in better function

            int ret_midi_msg = lf_queue_push(&thread_stuff->raylib_msg_queue, "midi_msg", (void*)&midi_msg, sizeof(MidiMsg));

            jack_ringbuffer_write(thread_stuff->jack_stuff->ringbuffer_audio, (void *)signal_buf, 1024*sizeof(float));
            jack_ringbuffer_write(thread_stuff->jack_stuff->ringbuffer_video, (void *)signal_buf, 1024*sizeof(float));
        } else {
            usleep(2000);
        }
    }
    // TODO free tone handler
    synth_model_clear(synth_model);
    printf("model_gen_signal_thread ended, Good bye! \n");
    return NULL;
}
