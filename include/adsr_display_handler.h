#ifndef ADSR_DISPLAY_HANDLER_H_
#define ADSR_DISPLAY_HANDLER_H_

#include "adsr_display_msg.h"

typedef struct ADSRDisplayMap {
    int key;
    ADSRDisplayMsg value;
} ADSRDisplayMap;

typedef struct ADSRDisplayHandler {
    ADSRDisplayMap* adsr_display_map;
} ADSRDisplayHandler;

void set_adsr_display_wrapper(void* adsr_display_msg_raw, void* adsr_display_handler_raw);
void set_adsr_display(ADSRDisplayHandler *adsr_display_handler, ADSRDisplayMsg *adsr_display_msg);
void adsr_display_handler_cleanup(ADSRDisplayHandler *adsr_display_handler);
void print_adsr_display_hash_map(ADSRDisplayHandler *adsr_display_handler);

#endif // ADSR_DISPLAY_HANDLER_H
