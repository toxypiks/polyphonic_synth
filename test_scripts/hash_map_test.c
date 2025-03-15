#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <stdio.h>

void main() {
    struct { int key; float value; } *myhash = NULL;
    hmput(myhash, 123, 0.1);
    hmput(myhash, 13, 1.9);
    hmput(myhash, 8, 12.56);

    int length = hmlen(myhash);
    printf("hm length: %d\n", length);
    hmfree(myhash);
}
