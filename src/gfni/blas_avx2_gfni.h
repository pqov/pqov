// SPDX-License-Identifier: CC0 OR Apache-2.0
/// @file blas_avx2_gfni.h
/// @brief Inlined functions for implementing basic linear algebra functions for GFNI arch.
///

#ifndef _BLAS_AVX2_GFNI_H_
#define _BLAS_AVX2_GFNI_H_

#include "gf16.h"

#include <immintrin.h>

#include "config.h"   // _GE_CONST_TIME_CADD_EARLY_STOP_
//#include "assert.h"

#include "gf16_gfni.h"

#include "blas_avx2.h"

static inline
void gf16v_generate_multabs_gfni( uint8_t *_multabs, const uint8_t *v, unsigned n_ele ) {
    __m256i * multabs = (__m256i *)_multabs;
    __m256i mat_bitsplit = _mm256_load_si256((const __m256i*)__split_low_4bits);
    __m256i mat_mulbit   = _mm256_load_si256((const __m256i*)__gf16_mulbit);

    while( n_ele > 1 ) {
        __m256i v0 = _mm256_set1_epi8(v[0]);
        __m256i v1 = _mm256_set1_epi8(v[0]>>4);
        __m256i b40 = _mm256_gf2p8affine_epi64_epi8(v0, mat_bitsplit , 0 );
        __m256i b41 = _mm256_gf2p8affine_epi64_epi8(v1, mat_bitsplit , 0 );
        __m256i tab_v0 = mat_mulbit&b40;
        __m256i tab_v1 = mat_mulbit&b41;
        __m256i tab_r1 = _mm256_permute4x64_epi64(tab_v0,0x39);  // 0,3, 2,1
        __m256i tab_r2 = _mm256_permute4x64_epi64(tab_v0,0x4e);  // 1,0, 3,2
        __m256i tab_r3 = _mm256_permute4x64_epi64(tab_v0,0x93);  // 2,1, 0,3
        __m256i tab_s1 = _mm256_permute4x64_epi64(tab_v1,0x39);  // 0,3, 2,1
        __m256i tab_s2 = _mm256_permute4x64_epi64(tab_v1,0x4e);  // 1,0, 3,2
        __m256i tab_s3 = _mm256_permute4x64_epi64(tab_v1,0x93);  // 2,1, 0,3
        multabs[0] = tab_v0^tab_r1^tab_r2^tab_r3;
        multabs[1] = tab_v1^tab_s1^tab_s2^tab_s3;
        n_ele -= 2;
        v += 1;
        multabs += 2;
    }
    if (n_ele) {
        __m256i v0 = _mm256_set1_epi8(v[0]);
        __m256i b40 = _mm256_gf2p8affine_epi64_epi8(v0, mat_bitsplit , 0 );
        __m256i tab_v0 = mat_mulbit&b40;
        __m256i tab_r1 = _mm256_permute4x64_epi64(tab_v0,0x39);  // 0,3, 2,1
        __m256i tab_r2 = _mm256_permute4x64_epi64(tab_v0,0x4e);  // 1,0, 3,2
        __m256i tab_r3 = _mm256_permute4x64_epi64(tab_v0,0x93);  // 2,1, 0,3
        multabs[0] = tab_v0^tab_r1^tab_r2^tab_r3;
    }
}

static inline
void gf16v_madd_multab_gfni( uint8_t *accu_c, const uint8_t *a, const uint8_t *multab, unsigned _num_byte ) {
    __m256i mat_mul = _mm256_load_si256((const __m256i*)multab);
    unsigned n_32 = _num_byte >> 5;
    unsigned rem = _num_byte & 31;
    if ( rem ) {
        if (n_32 ) {
            __m256i inp = _mm256_loadu_si256( (__m256i *)a );
            __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
            __m256i ou1 = _mm256_loadu_si256( (__m256i *)(accu_c+rem) );
            __m256i r0 = out ^ _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
            _mm256_storeu_si256( (__m256i *)accu_c, r0 );
            _mm256_storeu_si256( (__m256i *)(accu_c+rem), ou1 );
        } else {
            __m256i inp = _load_ymm( a , rem );
            __m256i out = _load_ymm( accu_c , rem );
            __m256i r0 = out ^ _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
            _store_ymm( accu_c , rem , r0 );
        }
        a += rem;
        accu_c += rem;
    }
    while (n_32--) {
        __m256i inp = _mm256_loadu_si256( (__m256i *)a );
        __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
        __m256i r0 = out ^ _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
        _mm256_storeu_si256( (__m256i *)accu_c, r0 );
        a += 32;
        accu_c += 32;
    }
}

static inline
void gf16v_madd_gfni( uint8_t *accu_c, const uint8_t *a, uint8_t b, unsigned _num_byte ) {
    __m256i mat_mul = gf16v_get_multab_gfni( b );
    unsigned n_32 = _num_byte >> 5;
    unsigned rem = _num_byte & 31;
    if ( rem ) {
        if (n_32 ) {
            __m256i inp = _mm256_loadu_si256( (__m256i *)a );
            __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
            __m256i ou1 = _mm256_loadu_si256( (__m256i *)(accu_c+rem) );
            __m256i r0 = out ^ _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
            _mm256_storeu_si256( (__m256i *)accu_c, r0 );
            _mm256_storeu_si256( (__m256i *)(accu_c+rem), ou1 );
        } else {
            __m256i inp = _load_ymm( a , rem );
            __m256i out = _load_ymm( accu_c , rem );
            __m256i r0 = out ^ _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
            _store_ymm( accu_c , rem , r0 );
        }
        a += rem;
        accu_c += rem;
    }
    while (n_32--) {
        __m256i inp = _mm256_loadu_si256( (__m256i *)a );
        __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
        __m256i r0 = out ^ _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
        _mm256_storeu_si256( (__m256i *)accu_c, r0 );
        a += 32;
        accu_c += 32;
    }
}

static inline
void gf16v_mul_scalar_gfni( uint8_t *a, uint8_t b, unsigned _num_byte ) {
    __m256i mat_mul = gf16v_get_multab_gfni( b );
    unsigned n_32 = _num_byte >> 5;
    unsigned rem = _num_byte & 31;
    if ( rem ) {
        if (n_32 ) {
            __m256i inp = _mm256_loadu_si256( (__m256i *)a );
            __m256i ou1 = _mm256_loadu_si256( (__m256i *)(a+rem) );
            __m256i r0 = _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
            _mm256_storeu_si256( (__m256i *) a, r0 );
            _mm256_storeu_si256( (__m256i *)(a+rem), ou1 );
        } else {
            __m256i inp = _load_ymm( a , rem );
            __m256i r0 = _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
            _store_ymm( a , rem , r0 );
        }
        a += rem;
    }
    while (n_32--) {
        __m256i inp = _mm256_loadu_si256( (__m256i *)a );
        __m256i r0 = _mm256_gf2p8affine_epi64_epi8( inp , mat_mul , 0 );
        _mm256_storeu_si256( (__m256i *)a, r0 );
        a += 32;
    }
}





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

