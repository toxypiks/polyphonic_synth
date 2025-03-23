#ifndef TONE_HANDLER_H_
#define  TONE_HANDLER_H_

#include "synth_model.h"
#include "midi_msg.h"

#include "stb_ds.h"

typedef struct ToneMap {
    int key;
    SynthModel value;
} ToneMap;


typedef struct ToneHandler {
    ToneMap* tone_map;
} ToneHandler;

// TODO ToneHandler as first parameter fix msg_handler interface
void set_tone(MidiMsg *midi_msg_new, ToneHandler *tone_handler) {
    int key = midi_msg_new->key;
    int index = hmgeti(tone_handler->tone_map, key);
    printf("set_midi_msg_in_map index: %d \n", index);
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
#endif // TONE_HANDLER_H_

int tone_handler_len(ToneHandler *tone_handler)
{
    return hmlen(tone_handler->tone_map);
}
