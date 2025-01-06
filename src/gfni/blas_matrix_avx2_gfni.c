/// @file blas_comm_avx2.c
/// @brief Implementations for blas_comm_avx2.h
///

#include "gf16.h"

#include "config.h"

#include "gf16_avx2.h"

#include "blas_avx2.h"

#include "blas_comm.h"

#include "blas_matrix_avx2.h"

#include "blas_matrix_avx2_gfni.h"

#include <immintrin.h>

#include "string.h"


#include "params.h"  // for macro _USE_GF16


#define _GE_CADD_EARLY_STOP_



////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  matrix-vector multiplication, GF( 256 ) ///////////////////
////////////////////////////////////////////////////////////////////////////////////////////


// process bytes < 16.
// XXX: This function uses very slow load/store operations. I do not expect this will be called.
static void _gf256mat_prod_smaller_than_16( uint8_t *c, const uint8_t *matA, unsigned _num_byte, unsigned matA_vec_byte, unsigned matA_n_vec, const uint8_t *b ) {
    __m256i c0;
    {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _load_ymm((matA + 0), _num_byte);
        c0 = _mm256_gf2p8mul_epi8( a0, bb );
        matA += matA_vec_byte;
        b++;
    }
    for (unsigned i = 1; i < matA_n_vec; i++) {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _load_ymm((matA + 0), _num_byte);
        c0 ^= _mm256_gf2p8mul_epi8( a0, bb );
        matA += matA_vec_byte;
        b++;
    }
    _store_ymm(c, _num_byte, c0);
}

// process bytes < 32
static void _gf256mat_prod_smaller( uint8_t *c, const uint8_t *matA, unsigned _num_byte, unsigned matA_vec_byte, unsigned matA_n_vec, const uint8_t *b ) {
    __m256i c0;
    {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        c0 = _mm256_gf2p8mul_epi8( a0, bb );
        matA += matA_vec_byte;
        b++;
    }
    for (unsigned i = 1; i < matA_n_vec - 1; i++) {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        c0 ^= _mm256_gf2p8mul_epi8( a0, bb );
        matA += matA_vec_byte;
        b++;
    }
    {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _load_ymm((matA + 0), _num_byte);
        c0 ^= _mm256_gf2p8mul_epi8( a0, bb );
    }
    _store_ymm(c, _num_byte, c0);
}

// process bytes < 32
static void _gf256mat_prod_small( uint8_t *c, const uint8_t *matA, unsigned _num_byte, unsigned matA_vec_byte, unsigned matA_n_vec, const uint8_t *b ) {
    __m256i c0;
    {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        c0 = _mm256_gf2p8mul_epi8( a0, bb );
        matA += matA_vec_byte;
        b++;
    }
    for (unsigned i = 1; i < matA_n_vec; i++) {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        c0 ^= _mm256_gf2p8mul_epi8( a0, bb );
        matA += matA_vec_byte;
        b++;
    }
    _store_ymm(c, _num_byte, c0);
}

static void _gf256mat_prod_32byte( uint8_t *c, const uint8_t *matA, unsigned matA_vec_byte, unsigned matA_n_vec, const uint8_t *b ) {
    __m256i c0;
    {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        c0 = _mm256_gf2p8mul_epi8( a0, bb );
        matA += matA_vec_byte;
        b++;
    }
    for (unsigned i = 1; i < matA_n_vec; i++) {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        c0 ^= _mm256_gf2p8mul_epi8( a0, bb );
        matA += matA_vec_byte;
        b++;
    }
    _mm256_storeu_si256((__m256i *)(c + 0), c0);
}

static void _gf256mat_prod_64byte( uint8_t *c, const uint8_t *matA, unsigned matA_vec_byte, unsigned matA_n_vec, const uint8_t *b ) {
    __m256i c0, c1;
    {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        __m256i a1 = _mm256_loadu_si256((__m256i *)(matA + 32));
        c0 = _mm256_gf2p8mul_epi8( a0, bb );
        c1 = _mm256_gf2p8mul_epi8( a1, bb );
        matA += matA_vec_byte;
        b++;
    }
    for (unsigned i = 1; i < matA_n_vec; i++) {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        __m256i a1 = _mm256_loadu_si256((__m256i *)(matA + 32));
        c0 ^= _mm256_gf2p8mul_epi8( a0, bb );
        c1 ^= _mm256_gf2p8mul_epi8( a1, bb );
        matA += matA_vec_byte;
        b++;
    }
    _mm256_storeu_si256((__m256i *)(c + 0), c0);
    _mm256_storeu_si256((__m256i *)(c + 32), c1);
}

static void _gf256mat_prod_96byte( uint8_t *c, const uint8_t *matA, unsigned matA_vec_byte, unsigned matA_n_vec, const uint8_t *b ) {
    __m256i c0, c1, c2;
    {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        __m256i a1 = _mm256_loadu_si256((__m256i *)(matA + 32));
        __m256i a2 = _mm256_loadu_si256((__m256i *)(matA + 64));
        c0 = _mm256_gf2p8mul_epi8( a0, bb );
        c1 = _mm256_gf2p8mul_epi8( a1, bb );
        c2 = _mm256_gf2p8mul_epi8( a2, bb );
        matA += matA_vec_byte;
        b++;
    }
    for (unsigned i = 1; i < matA_n_vec; i++) {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        __m256i a1 = _mm256_loadu_si256((__m256i *)(matA + 32));
        __m256i a2 = _mm256_loadu_si256((__m256i *)(matA + 64));
        c0 ^= _mm256_gf2p8mul_epi8( a0, bb );
        c1 ^= _mm256_gf2p8mul_epi8( a1, bb );
        c2 ^= _mm256_gf2p8mul_epi8( a2, bb );
        matA += matA_vec_byte;
        b++;
    }
    _mm256_storeu_si256((__m256i *)(c + 0), c0);
    _mm256_storeu_si256((__m256i *)(c + 32), c1);
    _mm256_storeu_si256((__m256i *)(c + 64), c2);
}

static void _gf256mat_prod_128byte( uint8_t *c, const uint8_t *matA, unsigned matA_vec_byte, unsigned matA_n_vec, const uint8_t *b ) {
    __m256i c0, c1, c2, c3;
    {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        __m256i a1 = _mm256_loadu_si256((__m256i *)(matA + 32));
        __m256i a2 = _mm256_loadu_si256((__m256i *)(matA + 64));
        __m256i a3 = _mm256_loadu_si256((__m256i *)(matA + 96));
        c0 = _mm256_gf2p8mul_epi8( a0, bb );
        c1 = _mm256_gf2p8mul_epi8( a1, bb );
        c2 = _mm256_gf2p8mul_epi8( a2, bb );
        c3 = _mm256_gf2p8mul_epi8( a3, bb );
        matA += matA_vec_byte;
        b++;
    }
    for (unsigned i = 1; i < matA_n_vec; i++) {
        __m256i bb = _mm256_set1_epi8(b[0]);
        __m256i a0 = _mm256_loadu_si256((__m256i *)(matA + 0));
        __m256i a1 = _mm256_loadu_si256((__m256i *)(matA + 32));
        __m256i a2 = _mm256_loadu_si256((__m256i *)(matA + 64));
        __m256i a3 = _mm256_loadu_si256((__m256i *)(matA + 96));
        c0 ^= _mm256_gf2p8mul_epi8( a0, bb );
        c1 ^= _mm256_gf2p8mul_epi8( a1, bb );
        c2 ^= _mm256_gf2p8mul_epi8( a2, bb );
        c3 ^= _mm256_gf2p8mul_epi8( a3, bb );
        matA += matA_vec_byte;
        b++;
    }
    _mm256_storeu_si256((__m256i *)(c + 0), c0);
    _mm256_storeu_si256((__m256i *)(c + 32), c1);
    _mm256_storeu_si256((__m256i *)(c + 64), c2);
    _mm256_storeu_si256((__m256i *)(c + 96), c3);
}


void gf256mat_prod_avx2_gfni( uint8_t *c, const uint8_t *matA, unsigned matA_vec_byte, unsigned matA_n_vec, const uint8_t *b ) {

    unsigned num_byte = matA_vec_byte;
    if ( num_byte & 31 ) {
        unsigned rem = num_byte & 31;
        if ( num_byte < 32 ) {
            if ( num_byte < 16 ) {
                _gf256mat_prod_smaller_than_16( c, matA, rem, matA_vec_byte, matA_n_vec, b );
            } else {
                _gf256mat_prod_smaller( c, matA, rem, matA_vec_byte, matA_n_vec, b );
            }
        } else {
            _gf256mat_prod_small( c, matA, rem, matA_vec_byte, matA_n_vec, b );
        }
        matA += rem;
        c += rem;
        num_byte -= rem;
    }
    while ( num_byte >= 128 ) {
        _gf256mat_prod_128byte( c, matA, matA_vec_byte, matA_n_vec, b );
        matA += 128;
        c += 128;
        num_byte -= 128;
    }
    if ( num_byte >= 96 ) {
        _gf256mat_prod_96byte( c, matA, matA_vec_byte, matA_n_vec, b );
        matA += 96;
        c += 96;
        num_byte -= 96;
    } else if ( num_byte >= 64 ) {
        _gf256mat_prod_64byte( c, matA, matA_vec_byte, matA_n_vec, b );
        matA += 64;
        c += 64;
        num_byte -= 64;
    } else if ( num_byte >= 32 ) {
        _gf256mat_prod_32byte( c, matA, matA_vec_byte, matA_n_vec, b );
        matA += 32;
        c += 32;
        num_byte -= 32;
    }
}



//////////////////////////////////////////////////////////////////////////////////////
///////////////////  code for solving linear equations,  GF(256) /////////////////////
//////////////////////////////////////////////////////////////////////////////////////



//////////////////    Gaussian elimination + Back substitution for solving linear equations  //////////////////


static
unsigned _gf256mat_gauss_elim_row_echelon_avx2_gfni( uint8_t *mat, unsigned h, unsigned w, unsigned offset ) {
    // assert( (w&31)==0 );
    unsigned n_ymm = w >> 5;
    __m128i mask_0 = _mm_setzero_si128();

    // assert( h <= 128 );
#define MAX_H  96
    uint8_t pivots[MAX_H] __attribute__((aligned(32)));
#undef MAX_H

    uint8_t rr8 = 1;
    for (unsigned i = 0; i < h; i++) {
        unsigned idx = offset + i;
        unsigned char i_r16 = idx & 0xf;
        unsigned i_d16 = idx >> 4;
        unsigned i_d32 = idx >> 5;

        uint8_t *mi = mat + i * w;

        #if defined( _GE_CADD_EARLY_STOP_ )
        unsigned stop = (i + 8 < h) ? i + 8 : h;
        for (unsigned j = i + 1; j < stop; j++) {
        #else
        for (unsigned j = i + 1; j < h; j++) {
        #endif
            __m128i piv_i   = _mm_load_si128( (__m128i *)( mi + i_d16 * 16 ) );
            __m128i is_zero = _mm_cmpeq_epi8( piv_i, mask_0 );
            __m128i add_mask = _mm_shuffle_epi8( is_zero, _mm_set1_epi8(i_r16) );

            __m256i mask = _mm256_setr_m128i( add_mask, add_mask );
            uint8_t *mj = mat + j * w;

            for (unsigned k = i_d32; k < n_ymm; k++) {
                __m256i p_i = _mm256_load_si256( (__m256i *)( mi + k * 32 ) );
                __m256i p_j = _mm256_load_si256( (__m256i *)( mj + k * 32 ) );

                p_i ^= mask & p_j;
                _mm256_store_si256( (__m256i *)( mi + k * 32 ), p_i );
            }
        }
        for (unsigned j = i; j < h; j++) {
            pivots[j] = mat[j * w + idx];
        }
        rr8 &= gf256_is_nonzero( pivots[i] );
        pivots[i] = gf256_inv_sse( pivots[i] );

        // pivot row
        gf256v_mul_scalar_avx2_gfni( mi + i_d32 * 32, pivots[i], (n_ymm - i_d32) << 5 );
        // eliminate rows
        for (unsigned j = i + 1; j < h; j++) {
            gf256v_madd_avx2_gfni( mat + j * w + i_d32 * 32, mi + i_d32 * 32, pivots[j], (n_ymm - i_d32) << 5 );
        }
    }
    return rr8;
}



unsigned gf256mat_gaussian_elim_avx2_gfni(uint8_t *sqmat_a, uint8_t *constant, unsigned len) {
#define MAX_H  96
    uint8_t mat[MAX_H * (MAX_H + 32)] __attribute__((aligned(32)));
#undef MAX_H

    unsigned height = len;
    unsigned width  = ((len + 1 + 31) >> 5) << 5;
    unsigned offset = width - (len + 1);
    for (unsigned i = 0; i < height; i++) {
        uint8_t *ai = mat + i * width;
        for (unsigned j = 0; j < height; j++) {
            ai[offset + j] = sqmat_a[j * len + i];    // transpose since sqmat_a is col-major
        }
        ai[offset + height] = constant[i];
    }
    unsigned char r8 =  _gf256mat_gauss_elim_row_echelon_avx2_gfni( mat, height, width, offset );

    for (unsigned i = 0; i < height; i++) {
        uint8_t *ai = mat + i * width;
        memcpy( sqmat_a + i * len, ai + offset, len );   // output a row-major matrix
        constant[i] = ai[offset + len];
    }
    return r8;
}


void gf256mat_back_substitute_avx2_gfni( uint8_t *constant, const uint8_t *sq_row_mat_a, unsigned len) {
    //const unsigned MAX_H=96;
#define MAX_H  96
    uint8_t column[MAX_H] __attribute__((aligned(32))) = {0};
    uint8_t temp[MAX_H] __attribute__((aligned(32)));
#undef MAX_H
    memcpy( temp, constant, len );
    for (int i = len - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            column[j] = sq_row_mat_a[j * len + i];    // row-major -> column-major, i.e., transpose
        }
        column[i] = 0;
        unsigned len_i = ((i + 31) >> 5) << 5;
        gf256v_madd_avx2_gfni( temp, column, temp[i], len_i );
    }
    memcpy( constant, temp, len );
}


