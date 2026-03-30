#include "midi_msg.h"
#include <string.h>
#include "ui_stuff.h"
#include "stb_ds.h"
#include <stdio.h>

void set_midi_msg(void* midi_msg_new_raw, void* midi_msg_raw)
{
    MidiMsg* midi_msg = (MidiMsg*)midi_msg_raw;
    MidiMsg* midi_msg_new = (MidiMsg*)midi_msg_new_raw;
    memcpy(midi_msg, midi_msg_new, sizeof(MidiMsg));
}

void set_midi_msgs_in_hmap(void* midi_msg_new_raw, void* key_map_raw)
{
    KeyboardPressedKeyMap** key_map = (KeyboardPressedKeyMap**)key_map_raw;
    MidiMsg* midi_msg_new = (MidiMsg*)midi_msg_new_raw;

    int key = midi_msg_new->key;
    if(midi_msg_new->is_on){
         bool value = true;
        hmput((*key_map), key, value);
        printf("set_midi_msg_in_hmap: key: %d\n", key);
    } else {
        // NoteOff is_on = false
        hmdel((*key_map), key);
    }
}
