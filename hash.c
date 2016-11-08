#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


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


typedef uint32_t uint32x2_t __attribute__ ((vector_size (8)));

typedef struct {
    // Two Dietzfelbinger hash functions
    uint32x2_t a;
    uint32x2_t b;
    uint32x2_t shift;
    // Displacement values
    uint8_t d[32];
} PerfectHash;


uint32_t eval_hash(PerfectHash h, uint32x2_t x) {
    uint32x2_t v = (h.a * x + h.b) >> h.shift;
    return v[0] ^ h.d[v[1]];
}


PerfectHash new_random_hash(unsigned int log_n_f, unsigned int log_n_g) {
    uint32x2_t shift = {32 - log_n_f, 32 - log_n_g};
    uint32x2_t b_mask = {(1 << shift[0]) - 1, (1 << shift[1]) - 1};
    unsigned int n_f = 1 << log_n_f;
    unsigned int n_g = 1 << log_n_g;
    printf("%u %u\n", n_f, n_g);
    uint32x2_t b = (uint32x2_t)rand64() & b_mask;
    uint32x2_t a = (uint32x2_t)rand64() | (uint32x2_t){1, 1};
    PerfectHash h = (PerfectHash){
        .a=a,
        .b=b,
        .shift=shift,
    };
    for (int i = 0; i < 32; i++) {
        h.d[i] = rand8_masked(n_g - 1);
    }
    return h;
}


PerfectHash find_perfect_hash(unsigned int n, uint32x2_t keys[]) {
    unsigned int log_n_f = (unsigned int)ceil(log2(n)) - 2;
    unsigned int log_n_g = log_n_f + 3;
    PerfectHash h;
retry:
    h = new_random_hash(log_n_f, log_n_g);
    printf("trying a={%u, %u} b={%u, %u} shift={%u, %u})\n", h.a[0], h.a[1], h.b[0], h.b[1], h.shift[0], h.shift[1]);
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < i; j++) {
            uint32_t v_i = eval_hash(h, keys[i]);
            uint32_t v_j = eval_hash(h, keys[j]);
            if (v_i == v_j) {
                printf("collision: h(%u) == h(%u) == %u i=%u j=%u\n", keys[i][0], keys[j][0], v_i, i, j);
                goto retry;
            }
        }
    }
    return h;
}


int main(int argc, char** argv) {
    srand(time(NULL));

    int N = 21;
    uint32x2_t keys[N];
    for (unsigned int i = 0; i < N; i++) {
        uint32_t key = rand64() & 0xFFFFFFFF;
        keys[i] = (uint32x2_t){key, key};
        printf("keys[%u] = %u\n", i, key);
    }
    PerfectHash h = find_perfect_hash(N, keys);
    printf("found a={%u, %u} b={%u, %u} shift={%u, %u})\n", h.a[0], h.a[1], h.b[0], h.b[1], h.shift[0], h.shift[1]);

    uint64_t s = 0;
    int start = time(NULL);
    for (unsigned long k = 0; k < 100000000L; k++) {
        for (unsigned int i = 0; i < N; i++) {
            s += eval_hash(h, keys[i]);
        }
    }
    printf("%lu bytes; %lis\n", sizeof(PerfectHash), time(NULL) - start);
    printf("%llu\n", s);
}
