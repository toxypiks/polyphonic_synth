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

void set_tone_wrapper(void* midi_msg_new_raw, void* tone_handler_raw);
// TODO ToneHandler as first parameter fix msg_handler interface
void set_tone(MidiMsg *midi_msg_new, ToneHandler *tone_handler);

int tone_handler_len(ToneHandler *tone_handler);

#endif // TONE_HANDLER_H_
