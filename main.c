#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "randutil.h"
#include "timing.h"
#include "rp99.h"


int main(int argc, char** argv) {
    srand(time(NULL));
    unsigned long iterations = 10e3;
    unsigned long prep_count = 100;
    unsigned int item_count = 21;

    unsigned long prep_cycles = 0;
    unsigned long lookup_cycles = 0;

    uint64_t s = 0;
    uint32_t keys[item_count];
    struct timespec start, stop;
    double ns = 0;

    for (unsigned long j = 0; j < prep_count; j++) {
        for (unsigned int i = 0; i < item_count; i++) {
            keys[i] = rand64() & 0xFFFFFFFF;
            //printf("keys[%u] = %u\n", i, keys[i]);
        }

        clock_t start_prep = clock();
        RP99XorHash h = find_rp99xor_hash(item_count, keys);
        prep_cycles += clock() - start_prep;
        //printf("found a={%u, %u} b={%u, %u} shift={%u, %u})\n", h.a[0], h.a[1], h.b[0], h.b[1], h.shift[0], h.shift[1]);

        for (unsigned long k = 0; k < iterations; k++) {
            clock_t start_lookup = clock();
            for (unsigned int i = 0; i < item_count; i++) {
                //clock_t start_eval = clock();
                s += eval_rp99xor_hash(h, keys[i]);
                //lookup_cycles += clock() - start_eval;
            }
            lookup_cycles += clock() - start_lookup;
        }
    }
    printf("%lu bytes\n", sizeof(RP99XorHash));
    printf("%llu\n", s);
    printf(
        "-> took %lu cycles, %.6f s; %.3f cycles/lookup, %.3f ns/lookup\n",
        lookup_cycles,
        ns,
        lookup_cycles / (double)iterations / (double)prep_count,
        ns * (1e9 / (double)iterations / (double)prep_count)
    );
    printf("-> took %lu cycles: %.3f cycles/prep\n", prep_cycles, prep_cycles / (double)prep_count);
}
