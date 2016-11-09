#include "rp99.h"


uint32_t eval_rp99xor_hash(RP99XorHash h, uint32_t x) {
    uint32x2_t xx = (uint32x2_t){x, x};
    uint32x2_t v = (h.a * xx + h.b) >> h.shift;
    return v[0] ^ h.d[v[1]];
}


RP99XorHash new_random_hash(unsigned int log_n_f, unsigned int log_n_g) {
    uint32x2_t shift = {32 - log_n_f, 32 - log_n_g};
    uint32x2_t b_mask = {(1 << shift[0]) - 1, (1 << shift[1]) - 1};
    unsigned int n_f = 1 << log_n_f;
    unsigned int n_g = 1 << log_n_g;
    //printf("%u %u\n", n_f, n_g);
    uint32x2_t b = (uint32x2_t)rand64() & b_mask;
    uint32x2_t a = (uint32x2_t)rand64() | (uint32x2_t){1, 1};
    RP99XorHash h = (RP99XorHash){
        .a=a,
        .b=b,
        .shift=shift,
    };
    for (int i = 0; i < 32; i++) {
        h.d[i] = rand8_masked(n_g - 1);
    }
    return h;
}


RP99XorHash find_rp99xor_hash(unsigned int n, uint32_t keys[]) {
    unsigned int log_n_f = (unsigned int)ceil(log2(n)) - 2;
    unsigned int log_n_g = log_n_f + 3;
    RP99XorHash h;
retry:
    h = new_random_hash(log_n_f, log_n_g);
    //printf("trying a={%u, %u} b={%u, %u} shift={%u, %u})\n", h.a[0], h.a[1], h.b[0], h.b[1], h.shift[0], h.shift[1]);
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < i; j++) {
            uint32_t v_i = eval_rp99xor_hash(h, keys[i]);
            uint32_t v_j = eval_rp99xor_hash(h, keys[j]);
            if (v_i == v_j) {
                //printf("collision: h(%u) == h(%u) == %u i=%u j=%u\n", keys[i], keys[j], v_i, i, j);
                goto retry;
            }
        }
    }
    return h;
}
