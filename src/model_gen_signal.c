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
    float adsr_height = 0.0;
    float adsr_length = 0.0;

    // msg_hdl_add_key2fct(&msg_hdl, "adsr", set_adsr_values, (void*)&adsr_values);
    msg_hdl_add_key2fct(&msg_hdl, "adsr", set_adsr_wrapper, (void*)&tone_handler);
    msg_hdl_add_key2fct(&msg_hdl, "vol", set_float_value, (void*)&vol);
    msg_hdl_add_key2fct(&msg_hdl, "midi_msg", set_tone_wrapper, (void*)&tone_handler);

    while(thread_stuff->is_running) {
        msg_hdling(&msg_hdl, &thread_stuff->model_msg_queue);
        msg_hdling(&msg_hdl, &thread_stuff->jack_stuff->midi_msg_queue);

        size_t num_bytes = jack_ringbuffer_read_space(thread_stuff->jack_stuff->ringbuffer_audio);
        float signal_buf[1024];

        tone_handler_retrigger(&tone_handler);
        if (num_bytes < 4800 * sizeof(float))
        {
            adsr_length = 0;
            /* TODO need to handle polyphonic MidiMsgs
            // TODO in future
            synth_model_envelope_update(synth_model,
                                        adsr_values.attack,
                                        adsr_values.decay,
                                        adsr_values.sustain,
                                        adsr_values.release,
                                        //is_play_pressed);
                                        midi_msg.is_on);

            */
            // for all synth models in
            float tone_buf[1024];
            int synth_model_length = tone_handler_len(&tone_handler);
            // printf("model_gen_signal(): synth_model_length: %d\n", synth_model_length);
            if (synth_model_length > 0) {
                for (size_t i = 0; i < synth_model_length; ++i) {
                    memset(tone_buf, 0, sizeof(1024));
                    synth_model_process(&tone_handler.tone_map[i].value,
                                        tone_buf,
                                        vol,
                                        &adsr_height,
                                        &adsr_length);
                    if (i == 0) {
                        for (size_t j = 0; j < 1024; j++) {
                            signal_buf[j] = tone_buf[j];
                        }
                    } else {
                        for (size_t j = 0; j < 1024; j++) {
                            // TODO: Better MIX
                            // bad formula= A + B - A*B
                            signal_buf[j] = (tone_buf[j] + signal_buf[j]) * 0.5;
                        }
                    }
                    // printf("model_gen_signal(): finish signal generation of i:%d\n", i);
                    /* TODO Idea on struct {key, adsr_length, adsr_height}
                    int ret_adsr_height = lf_queue_push(&thread_stuff->raylib_msg_queue, "adsr_height", (void*)&adsr_height, sizeof(float));
                    int ret_adsr_length = lf_queue_push(&thread_stuff->raylib_msg_queue, "adsr_length", (void*)&adsr_length, sizeof(float));
                    */
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
