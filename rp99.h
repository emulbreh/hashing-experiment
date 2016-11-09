#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "randutil.h"


typedef uint32_t uint32x2_t __attribute__ ((vector_size (8)));

typedef struct {
    // Two Dietzfelbinger hash functions
    uint32x2_t a;
    uint32x2_t b;
    uint32x2_t shift;
    // Displacement values
    uint8_t d[32];
} RP99XorHash;


uint32_t eval_rp99xor_hash(RP99XorHash h, uint32_t x);
RP99XorHash find_rp99xor_hash(unsigned int n, uint32_t keys[]);
