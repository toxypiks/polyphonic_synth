#include "adsr_display_handler.h"
#include "adsr_display_msg.h"
#include <stdio.h>
#include "stb_ds.h"

void set_adsr_display_wrapper(void* adsr_display_msg_raw, void* adsr_display_handler_raw) {
    ADSRDisplayMsg* adsr_display_msg = (ADSRDisplayMsg*)adsr_display_msg_raw;
    ADSRDisplayHandler* adsr_display_handler = (ADSRDisplayHandler*)adsr_display_handler_raw;
    set_adsr_display(adsr_display_handler, adsr_display_msg);
}

void set_adsr_display(ADSRDisplayHandler *adsr_display_handler, ADSRDisplayMsg *adsr_display_msg) {
    int key = adsr_display_msg->key;
    int index = hmgeti(adsr_display_handler->adsr_display_map, key);
    if (index < 0) {
        ADSRDisplayMsg adsr_display_msg_new = {
            .key = adsr_display_msg->key,
            .adsr_height = adsr_display_msg->adsr_height,
            .adsr_length = adsr_display_msg->adsr_length
        };
        hmput(adsr_display_handler->adsr_display_map, key, adsr_display_msg_new);
    } else {
        adsr_display_handler->adsr_display_map[index].value.adsr_height = adsr_display_msg->adsr_height;
        adsr_display_handler->adsr_display_map[index].value.adsr_length = adsr_display_msg->adsr_length;
    }
}

void adsr_display_handler_cleanup(ADSRDisplayHandler *adsr_display_handler) {
    // TODO check if length is over 1.0 or so...
    printf("implement me :) -> adsr_display_handler_cleanup\n");
}

void print_adsr_display_hash_map(ADSRDisplayHandler *adsr_display_handler)
{
    int adsr_display_handler_len = hmlen(adsr_display_handler->adsr_display_map);
    printf("print_hash_print(): adsr_display_handler %d items\n", adsr_display_handler_len);
    if (adsr_display_handler_len > 0) {
        for (size_t i = 0; i < adsr_display_handler_len; ++i) {
            printf("key: %d, adsr_length: %f, adsr_height: %f\n",
                   adsr_display_handler->adsr_display_map[i].key,
                   adsr_display_handler->adsr_display_map[i].value.adsr_length,
                   adsr_display_handler->adsr_display_map[i].value.adsr_height);
        }
    } else {
        printf("print_hash_print(): nothing in the hashmap\n");
    }
}
