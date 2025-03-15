#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <stdio.h>

typedef struct KeyValue {
    int key;
    float value;
} KeyValue;

void main() {
    KeyValue *myhash = NULL;
    hmput(myhash, 1, 0.1);
    hmput(myhash, 3, 1.9);
    hmput(myhash, 8, 12.56);
    hmput(myhash, 15, 14.56);

    int length = hmlen(myhash);
    printf("hm length: %d\n", length);

    for (size_t i = 0; i < 10; ++i) {
        hmput(myhash, i, i/10.0);
    }

    length = hmlen(myhash);
    printf("number of elements: %d\n", length);

    for (size_t i = 0; i < length; i++) {
        KeyValue pair = myhash[i];
        float value = hmget(myhash, pair.key);
        printf("key %d\n", pair.key);
        printf("value %f\n", value);
        printf("value from pair %f\n", pair.value);
    }

    hmdel(myhash, 4);
    hmdel(myhash, 5);
    printf(" ======== delete stuff ========\n");
    length = hmlen(myhash);
    printf("number of elements: %d\n", length);

    for (size_t i = 0; i < length; i++) {
        KeyValue pair = myhash[i];
        float value = hmget(myhash, pair.key);
        printf("key %d\n", pair.key);
        printf("value %f\n", value);
        printf("value from pair %f\n", pair.value);
    }

    hmfree(myhash);
}
