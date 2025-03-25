#include "tone_handler.h"
#include <math.h>
#include <stdio.h>

void set_tone_wrapper(void* midi_msg_new_raw, void* tone_handler_raw){
    MidiMsg *midi_msg_new = midi_msg_new_raw;
    ToneHandler *tone_handler = tone_handler_raw;
    set_tone(midi_msg_new, tone_handler);
}

void set_tone(MidiMsg *midi_msg_new, ToneHandler *tone_handler) {
    int key = midi_msg_new->key;
    int index = hmgeti(tone_handler->tone_map, key);
    if (index < 0) {
        // create new entry
        SynthModel synth_model = {
            .osc = {
                .amp = {0.0f},
                .freq = 440.0 * pow(2.0, ((float)key - 33.0)/12.0),
                .phase = 0.0f,
                .vel = midi_msg_new->vel,
                .is_on = midi_msg_new->is_on,
            },
            .adsr_envelop = {
                .envelop_state = PRESSED_ATTACK,
                .sample_count = 0,
                .sample_count_release = 0,
                .current_value = 0.0f,
                .attack = 0.05f, // TODO take set values
                .decay = 0.15f,
                .sustain = 0.5f,
                .release = 0.3f
            }
        };
        hmput(tone_handler->tone_map, key, synth_model);
    } else {
        tone_handler->tone_map[index].value.osc.is_on = midi_msg_new->is_on;
    }
}

int tone_handler_len(ToneHandler *tone_handler)
{
    return hmlen(tone_handler->tone_map);
}

void tone_handler_cleanup(ToneHandler *tone_handler) {
    int length = tone_handler_len(tone_handler);
    if(length < 0) return;
    for (int i = 0; i < length; ++i) {
        if(!tone_handler->tone_map[i].value.osc.is_on){
            int key = tone_handler->tone_map[i].key;
            hmdel(tone_handler->tone_map, key);
        }
    }
}
