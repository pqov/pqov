/// @file blas_avx2_gfni.h
/// @brief Inlined functions for implementing basic linear algebra functions for GFNI arch.
///

#ifndef _BLAS_AVX2_GFNI_H_
#define _BLAS_AVX2_GFNI_H_

#include "gf16.h"

#include <immintrin.h>

#include "config.h"
//#include "assert.h"

#include "gf16_avx2.h"

#include "blas_avx2.h"


///////////////////////   basic functions   /////////////////////////////////////

static inline
void gf256v_mul_scalar_avx2_gfni( uint8_t *a, uint8_t _b, unsigned _num_byte ) {
    __m256i bb = _mm256_set1_epi8(_b);
    if ( _num_byte & 31 ) {
        unsigned rem = _num_byte & 31;
        if ( _num_byte < 32 ) {
            __m256i aa = _load_ymm( a, rem );
            __m256i cc = _mm256_gf2p8mul_epi8( aa, bb );
            _store_ymm( a, rem, cc );
        } else {
            __m256i aa = _mm256_loadu_si256( (__m256i *)a );
            __m256i a1 = _mm256_loadu_si256( (__m256i *)(a+rem) );
            __m256i cc = _mm256_gf2p8mul_epi8( aa, bb );
            _mm256_storeu_si256( (__m256i *)a, cc );
            _mm256_storeu_si256( (__m256i *)(a+rem), a1 );
        }
        a += rem;
        _num_byte -= rem;
    }
    while ( _num_byte ) {
        __m256i aa = _mm256_loadu_si256((__m256i *)a);
        __m256i cc = _mm256_gf2p8mul_epi8( aa, bb );
        _mm256_storeu_si256( (__m256i *)a, cc );
        a += 32;
        _num_byte -= 32;
    }
}


static inline
void gf256v_madd_avx2_gfni( uint8_t *accu_c, const uint8_t *a, uint8_t _b, unsigned _num_byte ) {
    __m256i bb = _mm256_set1_epi8(_b);
    if ( _num_byte & 31 ) {
        unsigned rem = _num_byte & 31;
        if ( _num_byte < 32 ) {
            __m256i aa = _load_ymm( a, rem );
            __m256i ac = _load_ymm( accu_c, rem );
            __m256i cc = _mm256_gf2p8mul_epi8( aa, bb );
            _store_ymm( accu_c, rem, ac ^ cc );
        } else {
            __m256i aa = _mm256_loadu_si256( (__m256i *)a );
            __m256i ac = _mm256_loadu_si256( (__m256i *)accu_c );
            __m256i c1 = _mm256_loadu_si256( (__m256i *)(accu_c+rem) );
            __m256i r0 = ac ^ _mm256_gf2p8mul_epi8( aa, bb );
            _mm256_storeu_si256( (__m256i *)accu_c, r0 );
            _mm256_storeu_si256( (__m256i *)(accu_c+rem), c1 );
        }
        a += rem;
        accu_c += rem;
        _num_byte -= rem;
    }
    while ( _num_byte ) {
        __m256i aa = _mm256_loadu_si256((__m256i *)a);
        __m256i ac = _mm256_loadu_si256((__m256i *)accu_c);
        __m256i cc = _mm256_gf2p8mul_epi8( aa, bb )^ac;
        _mm256_storeu_si256( (__m256i *)accu_c, cc );
        a += 32;
        accu_c += 32;
        _num_byte -= 32;
    }
}


#endif // _BLAS_AVX2_GFNI_H_

