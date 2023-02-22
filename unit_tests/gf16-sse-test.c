
#include <stdio.h>

#include "gf16.h"

#include "utils.h"

#include "string.h"


#include "config.h"

#if !(defined(_BLAS_SSE_)||defined(_BLAS_AVX2_))

int main(void) {
    printf("ERROR -- No defining _BLAS_SSE_ or _BLAS_AVX2_ in config.h\n\n");
    return -1;
}


#else   // !(defined(_BLAS_SSE_)||defined(_BLAS_AVX2_))

#include "gf16_sse.h"


//#define _BENCHMARK_INV_

#if defined(_BENCHMARK_INV_)

#include "benchmark.h"
#include "blas.h"
#include "blas_comm.h"

#endif

int main(void) {
    #if defined(_BENCHMARK_INV_)
    struct benchmark bm0, bm1;
    bm_init( &bm0 );
    bm_init( &bm1 );
    #endif

    printf("====== unit test ======\n");
    printf("Testing  consistency of basic arithmetic between ref and ssse3.\n\n" );


    printf("\n\n============= GF(16) ==============\n");

    uint8_t a, b, c;
    __m128i a_vec, b_vec, c_vec;
    uint8_t b2;
    uint8_t v16[16];

    int fail = 0;

    printf("testing square: ");
    for (unsigned i = 0; i < 16; i++) {
        a = i;
        b = gf16_squ( a );
        a_vec = _mm_set1_epi8( a );
        b_vec = tbl_gf16_squ( a_vec );
        _mm_storeu_si128( (__m128i *) v16, b_vec );

        if ( b != v16[0] ) {
            printf("a^2 : %x ^2->  %x : %x\n", a, b, v16[0] );
            fail = 1;
            break;
        }
    }
    if ( fail ) {
        printf("FAIL!\n\n");
        return -1;
    } else {
        printf("PASS\n\n");
    }

    printf("testing inverse: ");
    for (unsigned i = 0; i < 16; i++) {
        a = i;
        b = gf16_inv( a );
        b2 = gf16_inv_sse( a );

        if ( b != b2 ) {
            printf("a^-1 : %x ^-1->  %x : %x\n", a, b, b2 );
            fail = 1;
            break;
        }
    }
    if ( fail ) {
        printf("FAIL!\n\n");
        return -1;
    } else {
        printf("PASS\n\n");
    }


    printf("testing multiplication: ");
    for (unsigned i = 0; i < 16; i++) {
        for (unsigned j = 0; j < 16; j++) {
            a = i;
            b = j;
            c = gf16_mul( a, b );
            a_vec = _mm_set1_epi8( a );
            b_vec = _mm_set1_epi8( b );
            c_vec = tbl_gf16_mul( a_vec, b_vec );
            _mm_storeu_si128( (__m128i *) v16, c_vec );

            if ( c != v16[0] ) {
                printf("axb : %x x %x ->  %x : %x\n", a, b, c, v16[0] );
                fail = 1;
                break;
            }
        }
        if ( fail ) {
            break;
        }
    }
    if ( fail ) {
        printf("FAIL!\n\n");
        return -1;
    } else {
        printf("PASS\n\n");
    }




    printf("\n\n============= GF(256) ==============\n");


    printf("testing square: ");
    for (unsigned i = 0; i < 256; i++) {
        a = i;
        b = gf256_squ( a );
        a_vec = _mm_set1_epi8( a );
        b_vec = tbl_gf256_squ( a_vec );
        _mm_storeu_si128( (__m128i *) v16, b_vec );

        if ( b != v16[0] ) {
            printf("a^2 : %x ^2->  %x : %x\n", a, b, v16[0] );
            fail = 1;
            break;
        }
    }
    if ( fail ) {
        printf("FAIL!\n\n");
        return -1;
    } else {
        printf("PASS\n\n");
    }


    printf("testing inverse: ");
    for (unsigned i = 0; i < 256; i++) {
        a = i;
        b = gf256_inv( a );
        b2 = gf256_inv_sse( a );

        if ( b != b2 ) {
            printf("a^-1 : %x ^-1->  %x : %x\n", a, b, b2 );
            fail = 1;
            break;
        }
    }
    if ( fail ) {
        printf("FAIL!\n\n");
        return -1;
    } else {
        printf("PASS\n\n");
    }

    #if defined(_BENCHMARK_INV_)
    uint8_t vec0[256], vec1[256];
    BENCHMARK( bm0, {
        for (unsigned i = 0; i < 256; i++) vec0[i] = gf256_inv(i);
    });
    BENCHMARK( bm1, {
        for (unsigned i = 0; i < 256; i++) vec1[i] = gf256_inv_sse(i);
    });
    gf256v_add( vec0, vec1, 256 );
    if ( gf256v_is_zero(vec0, 256) ) {
        char msg[256];
        bm_dump( msg, 256, &bm0 );
        printf("gf256_inv(): %s\n", msg );
        bm_dump( msg, 256, &bm1 );
        printf("gf256_inv_sse(): %s\n", msg );
    }
    #endif

    printf("testing multiplication: ");
    for (unsigned i = 0; i < 256; i++) {
        for (unsigned j = 0; j < 256; j++) {
            a = i;
            b = j;
            c = gf256_mul( a, b );
            a_vec = _mm_set1_epi8( a );
            b_vec = _mm_set1_epi8( b );
            c_vec = tbl_gf256_mul_vv( a_vec, b_vec );
            _mm_storeu_si128( (__m128i *) v16, c_vec );

            if ( c != v16[0] ) {
                printf("axb : %x x %x ->  %x : %x\n", a, b, c, v16[0] );
                fail = 1;
                break;
            }
        }
        if ( fail ) {
            break;
        }
    }
    if ( fail ) {
        printf("FAIL!\n\n");
        return -1;
    } else {
        printf("PASS\n\n");
    }

    printf("All tests passed.\n");

    return 0;
}


#endif  // !(defined(_BLAS_SSE_)||defined(_BLAS_AVX2_))
