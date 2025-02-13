/// @file gf16_neon.h
/// @brief Inlined functions for implementing GF arithmetics for NEON instruction sets.
///

#ifndef _GF16_GFNI_H_
#define _GF16_GFNI_H_


#include <immintrin.h>

#include "gf16_avx2.h"

#include "stdint.h"


// gf16 := gf2[x]/(x^4+x+1)
static const unsigned char __gf16_rdtab_256[32] __attribute__((aligned(32))) = {
    0x00, 0x13, 0x26, 0x35, 0x4c, 0x5f, 0x6a, 0x79, 0x8b, 0x98, 0xad, 0xbe, 0xc7, 0xd4, 0xe1, 0xf2,
    0x00, 0x13, 0x26, 0x35, 0x4c, 0x5f, 0x6a, 0x79, 0x8b, 0x98, 0xad, 0xbe, 0xc7, 0xd4, 0xe1, 0xf2,
};


static inline
__m256i _gf16v_reduce_gfni( __m256i mb, __m256i rdtab, __m256i mask_f ) {
    return mb ^ _mm256_shuffle_epi8( rdtab , _mm256_srli_epi16(mb,4)&mask_f );
}


static const uint32_t __split_low_4bits[8] __attribute__((aligned(32))) = {
    0x01010101, 0x01010101, 0x02020202, 0x02020202, 0x04040404, 0x04040404, 0x08080808, 0x08080808,
};

static const uint32_t __gf16_mulbit[8] __attribute__((aligned(32))) = {
    0x10204080, 0x01020408, 0x80902040, 0x08090204, 0x40c09020, 0x040c0902, 0x2060c090, 0x02060c09,
};

static inline
__m256i gf16v_get_multab_gfni( uint8_t b ) {
    __m256i bb = _mm256_set1_epi8(b);
    __m256i b4 = _mm256_gf2p8affine_epi64_epi8(bb, _mm256_load_si256((const __m256i*)__split_low_4bits) , 0 );
    __m256i tab_b4 = _mm256_load_si256((const __m256i*)__gf16_mulbit)&b4;
    __m256i tab_r1 = _mm256_permute4x64_epi64(tab_b4,0x39);  // 0,3, 2,1
    __m256i tab_r2 = _mm256_permute4x64_epi64(tab_b4,0x4e);  // 1,0, 3,2
    __m256i tab_r3 = _mm256_permute4x64_epi64(tab_b4,0x93);  // 2,1, 0,3
    return tab_b4^tab_r1^tab_r2^tab_r3;
}


static inline
__m256i gf16v_mul_gfni( __m256i a, uint8_t b ) {
    return _mm256_gf2p8affine_epi64_epi8( a , gf16v_get_multab_gfni(b) , 0 );
}



#endif // _GF16_GFNI_H_
