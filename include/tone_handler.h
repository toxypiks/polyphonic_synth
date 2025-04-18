#ifndef TONE_HANDLER_H_
#define  TONE_HANDLER_H_

#include "synth_model.h"
#include "midi_msg.h"
#include "adsr.h"
#include "stb_ds.h"

typedef struct ToneMap {
    int key;
    SynthModel value;
} ToneMap;


typedef struct ToneHandler {
    ToneMap* tone_map;
    ADSR adsr_default;
} ToneHandler;

void set_tone_wrapper(void* midi_msg_new_raw, void* tone_handler_raw);
void set_adsr_wrapper(void* adsr_msg_raw, void* tone_handler_raw);
// TODO ToneHandler as first parameter fix msg_handler interface
void set_tone(MidiMsg *midi_msg_new, ToneHandler *tone_handler);
void set_adsr(ToneHandler *tone_handler, ADSR *adsr_msg);
void tone_handler_retrigger(ToneHandler *tone_handler);
int tone_handler_len(ToneHandler *tone_handler);
void print_tone_handler_hash_map(ToneHandler *tone_handler);
void tone_handler_cleanup(ToneHandler *tone_handler);
void tone_handler_free_hashmap(ToneHandler *tone_handler);
#endif // TONE_HANDLER_H_
