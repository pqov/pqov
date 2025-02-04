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

static inline
__m256i gf16v_get_multab_gfni( uint8_t b ) {
    __m256i mask_f  = _mm256_set1_epi8( 0xf );
    __m256i rdtab   = _mm256_load_si256((const __m256i*)__gf16_rdtab_256);

    __m256i bb = _mm256_set1_epi8(b);
    __m256i indices = _mm256_set_epi32( 0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100, 0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100 );
    __m256i mb = _mm256_gf2p8mul_epi8( indices , bb );

    return _gf16v_reduce_gfni(mb,rdtab,mask_f);
}


static inline
__m256i gf16v_mul_gfni( __m256i a, uint8_t b ) {
    __m256i mask_f  = _mm256_set1_epi8( 0xf );
    __m256i rdtab   = _mm256_load_si256((const __m256i*)__gf16_rdtab_256);

    __m256i bb = _mm256_set1_epi8(b);
    __m256i alb = _mm256_gf2p8mul_epi8( a&mask_f , bb );
    __m256i ahb = _mm256_gf2p8mul_epi8( _mm256_srli_epi16(a,4)&mask_f , bb );

    alb ^= _mm256_shuffle_epi8(rdtab, _mm256_srli_epi16(alb,4)&mask_f);
    ahb ^= _mm256_shuffle_epi8(rdtab, _mm256_srli_epi16(ahb,4)&mask_f);

    return _gf16v_reduce_gfni(alb,rdtab,mask_f)^_mm256_slli_epi16(_gf16v_reduce_gfni(ahb,rdtab,mask_f),4);
}



#endif // _GF16_GFNI_H_
