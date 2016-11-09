#include "randutil.h"


uint8_t rand8_masked(uint8_t mask) {
    return rand() & mask;
}

uint8_t rand8() {
    return rand8_masked(0xFF);
}

uint64_t rand64() {
    uint64_t r = 0;
    for (int i = 0; i < 8; i++) {
        r |= ((uint64_t)rand8()) << (8 * i);
    }
    return r;
}
