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


//
// Caution: multabs are different from ssse3 version
// ssse3:  [multab_low] [ multab_high ]
//         <-   16  ->  <-    16     ->
// avx2:   [         multab_low       ]
//         <---        32          --->

static inline
void _gf16v_generate_multabs_gfni( __m256i *multabs, __m128i vl , __m128i vh, unsigned n_ele , __m256i mask_f , __m256i rdtab , __m256i indices ) {
    __m128i m0 = _mm_setzero_si128();
    while (n_ele > 1) {
        __m128i vli = _mm_shuffle_epi8(vl,m0);
        __m128i vhi = _mm_shuffle_epi8(vh,m0);
        vl = _mm_srli_si128(vl,1);
        vh = _mm_srli_si128(vh,1);

        __m256i v2 = _mm256_inserti128_si256( _mm256_castsi128_si256(vli) , vhi , 1 );
        __m256i ab = _mm256_gf2p8mul_epi8( indices , v2 );
        __m256i tab2 = _gf16v_reduce_gfni( ab , rdtab , mask_f);

        __m256i mul0 = _mm256_permute2x128_si256(tab2,tab2,0x00);
        __m256i mul1 = _mm256_permute2x128_si256(tab2,tab2,0x11);

        _mm256_store_si256(multabs  , mul0);
        _mm256_store_si256(multabs+1, mul1);
        n_ele -= 2;
        multabs += 2;
    }
    if (n_ele) {
        __m128i vli = _mm_shuffle_epi8(vl,m0);
        __m256i v2 = _mm256_inserti128_si256( _mm256_castsi128_si256(vli) , vli , 1 );
        __m256i ab = _mm256_gf2p8mul_epi8( indices , v2 );
        __m256i tab2 = _gf16v_reduce_gfni( ab , rdtab , mask_f);
        _mm256_store_si256(multabs  , tab2);
        n_ele -= 1;
        multabs += 1;
    }
}

static inline
void gf16v_generate_multabs_gfni( uint8_t *_multabs, const uint8_t *v, unsigned n_ele ) {
    __m256i mask_f  = _mm256_set1_epi8( 0xf );
    __m256i rdtab   = _mm256_load_si256((const __m256i*)__gf16_rdtab_256);
    __m256i indices = _mm256_set_epi32( 0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100, 0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100 );
    __m128i xmm_f = _mm256_castsi256_si128(mask_f);
    __m256i * multabs = (__m256i *)_multabs;
    while( n_ele >= 32 ) {
        __m128i vv = _mm_loadu_si128( (const __m128i *) v);
        __m128i vv0 = vv&xmm_f;
        __m128i vv1 = _mm_srli_epi16(vv,4)&xmm_f;
        _gf16v_generate_multabs_gfni( multabs , vv0 , vv1 , 32 , mask_f , rdtab , indices );
        n_ele -= 32;
        v += 16;
        multabs += 32;
    }
    if (n_ele) {
        __m128i vv = _load_xmm(v,(n_ele+1)>>1);
        __m128i vv0 = vv&xmm_f;
        __m128i vv1 = _mm_srli_epi16(vv,4)&xmm_f;
        _gf16v_generate_multabs_gfni( multabs , vv0 , vv1 , n_ele , mask_f , rdtab , indices );
    }
}


static inline
void gf16v_mul_scalar_gfni( uint8_t *a, uint8_t gf16_b, unsigned _num_byte ) {
    __m256i mask = _mm256_set1_epi8(0xf);
    __m256i ml = gf16v_get_multab_gfni(gf16_b);
    __m256i mh = _mm256_slli_epi16( ml, 4 );
    linearmap_8x8_ymm( a, ml, mh, mask, _num_byte );
}



static inline
void gf16v_madd_gfni( uint8_t *accu_c, const uint8_t *a, uint8_t gf16_b, unsigned _num_byte ) {
    __m256i mask = _mm256_set1_epi8(0xf);
    __m256i ml = gf16v_get_multab_gfni(gf16_b);
    __m256i mh = _mm256_slli_epi16( ml, 4 );
    linearmap_8x8_accu_ymm( accu_c, a, ml, mh, mask, _num_byte );
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

