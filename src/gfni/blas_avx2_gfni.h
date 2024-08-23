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


//
// Currently, the gfni instruction set is always enabled in archs supporing avx-512, which supporting masked operations.
// So I use mask registers even that it requires avx-512 (not so ''avx-2'').
//


///////////////////////   basic functions   /////////////////////////////////////


static inline
void gf256v_add_avx2_gfni( uint8_t *accu_c, const uint8_t *a, unsigned _num_byte ) {
    while ( _num_byte >= 32 ) {
        __m256i aa = _mm256_loadu_si256((__m256i *)a);
        __m256i ac = _mm256_loadu_si256((__m256i *)accu_c);
        _mm256_storeu_si256( (__m256i *)accu_c, aa ^ ac );
        a += 32;
        accu_c += 32;
        _num_byte -= 32;
    }
    if ( _num_byte ) {
        __mmask32 mask = (1 << _num_byte) - 1;
        __m256i aa = _mm256_maskz_loadu_epi8(mask, (__m256i *)a);
        __m256i ac = _mm256_maskz_loadu_epi8(mask, (__m256i *)accu_c);
        _mm256_mask_storeu_epi8( (__m256i *)accu_c, mask, aa ^ ac );
    }
}


static inline
void gf256v_mul_scalar_avx2_gfni( uint8_t *a, uint8_t _b, unsigned _num_byte ) {
    __m256i bb = _mm256_set1_epi8(_b);
    while ( _num_byte >= 32 ) {
        __m256i aa = _mm256_loadu_si256((__m256i *)a);
        __m256i cc = _mm256_gf2p8mul_epi8( aa, bb );
        _mm256_storeu_si256( (__m256i *)a, cc );
        a += 32;
        _num_byte -= 32;
    }
    if ( _num_byte ) {
        __mmask32 mask = (1 << _num_byte) - 1;
        __m256i aa = _mm256_maskz_loadu_epi8(mask, (__m256i *)a);
        __m256i cc = _mm256_gf2p8mul_epi8( aa, bb );
        _mm256_mask_storeu_epi8( (__m256i *)a, mask, cc );
    }
}


static inline
void gf256v_madd_avx2_gfni( uint8_t *accu_c, const uint8_t *a, uint8_t _b, unsigned _num_byte ) {
    __m256i bb = _mm256_set1_epi8(_b);
    while ( _num_byte >= 32 ) {
        __m256i aa = _mm256_loadu_si256((__m256i *)a);
        __m256i ac = _mm256_loadu_si256((__m256i *)accu_c);
        __m256i cc = _mm256_gf2p8mul_epi8( aa, bb )^ac;
        _mm256_storeu_si256( (__m256i *)accu_c, cc );
        a += 32;
        accu_c += 32;
        _num_byte -= 32;
    }
    if ( _num_byte ) {
        __mmask32 mask = (1 << _num_byte) - 1;
        __m256i aa = _mm256_maskz_loadu_epi8(mask, (__m256i *)a);
        __m256i ac = _mm256_maskz_loadu_epi8(mask, (__m256i *)accu_c);
        __m256i cc = _mm256_gf2p8mul_epi8( aa, bb )^ac;
        _mm256_mask_storeu_epi8( (__m256i *)accu_c, mask, cc );
    }
}


#endif // _BLAS_AVX2_GFNI_H_

