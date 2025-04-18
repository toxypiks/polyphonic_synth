#include "tone_handler.h"
#include <math.h>
#include <stdio.h>

void set_tone_wrapper(void* midi_msg_new_raw, void* tone_handler_raw){
    MidiMsg *midi_msg_new = midi_msg_new_raw;
    ToneHandler *tone_handler = tone_handler_raw;
    set_tone(midi_msg_new, tone_handler);
}

void set_adsr_wrapper(void* adsr_msg_raw, void* tone_handler_raw) {
    ToneHandler* tone_handler = tone_handler_raw;
    ADSR* adsr_msg = adsr_msg_raw;
    set_adsr(tone_handler, adsr_msg);
}

void set_tone(MidiMsg *midi_msg_new, ToneHandler *tone_handler) {
    int key = midi_msg_new->key;
    int index = hmgeti(tone_handler->tone_map, key);
    if (index < 0) {
        SynthModel synth_model = {0};
        set_oscillator(&synth_model.osc,
                       440.0f * pow(2.0f, ((float)key - 69.0f)/12.0f),
                       0.0f,
                       midi_msg_new->vel,
                       midi_msg_new->is_on,
                       false,
                       SINE_WAVE);

        set_evelope(&synth_model.adsr_envelop,
                    tone_handler->adsr_default.attack,
                    tone_handler->adsr_default.decay,
                    tone_handler->adsr_default.sustain,
                    tone_handler->adsr_default.release);
        hmput(tone_handler->tone_map, key, synth_model);
    } else {
        tone_handler->tone_map[index].value.osc.is_on = midi_msg_new->is_on;
    }
}

void set_adsr(ToneHandler* tone_handler, ADSR* adsr_msg) {
    tone_handler->adsr_default.attack = adsr_msg->attack;
    tone_handler->adsr_default.decay = adsr_msg->decay;
    tone_handler->adsr_default.sustain = adsr_msg->sustain;
    tone_handler->adsr_default.release = adsr_msg->release;
}

int tone_handler_len(ToneHandler *tone_handler)
{
    return hmlen(tone_handler->tone_map);
}

void tone_handler_retrigger(ToneHandler *tone_handler) {
    int length = tone_handler_len(tone_handler);
    if(length <= 0) return;
    for (int i = 0; i < length; ++i) {
        bool is_triggered = tone_handler->tone_map[i].value.osc.is_on;
        envelop_trigger(&tone_handler->tone_map[i].value.adsr_envelop, is_triggered);
    }
}

void tone_handler_cleanup(ToneHandler *tone_handler) {
    int length = tone_handler_len(tone_handler);
    if(length < 0) return;
    for (int i = 0; i < length; ++i) {
        int key = tone_handler->tone_map[i].key;
        if(tone_handler->tone_map[i].value.osc.is_end){
            int key = tone_handler->tone_map[i].key;
            hmdel(tone_handler->tone_map, key);
        }
    }
}

void print_tone_handler_hash_map(ToneHandler *tone_handler)
{
    int tone_handler_len = hmlen(tone_handler->tone_map);
    printf("print_hash_print(): tone_handler %d items\n",tone_handler_len);
    if (tone_handler_len > 0) {
        for (size_t i = 0; i < tone_handler_len; ++i) {
            printf("key: %d, is on: %d\n", tone_handler->tone_map[i].key, tone_handler->tone_map[i].value.osc.is_on);
        }
    } else {
        printf("print_hash_print(): nothing in the hashmap\n");
    }
}

void tone_handler_free_hashmap(ToneHandler *tone_handler) {
    hmfree(tone_handler->tone_map);
}
