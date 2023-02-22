
#include <stdio.h>

#include "gf16.h"

#include "utils.h"

#include "string.h"


#include "config.h"

#if !defined(_BLAS_AVX2_)

int main(void) {
    printf("ERROR -- No defining _BLAS_AVX2_ in config.h\n\n");
    return -1;
}


#else   // !defined(_BLAS_AVX2_)


#include "gf16_avx2.h"




int main() {

    printf("====== unit test ======\n");
    printf("Testing  consistency of basic arithmetic between ref and avx2.\n\n" );


    printf("\n\n============= GF(16) ==============\n");

    uint8_t a, b, c;
    __m256i a_vec, b_vec, c_vec;
    uint8_t v32[32];

    int fail = 0;

    printf("testing square: ");
    for (unsigned i = 0; i < 16; i++) {
        a = i;
        b = gf16_squ( a );
        a_vec = _mm256_set1_epi8( a );
        b_vec = tbl32_gf16_squ( a_vec );
        _mm256_storeu_si256( (__m256i *) v32, b_vec );

        if ( b != v32[0] ) {
            printf("a^2 : %x ^2->  %x : %x\n", a, b, v32[0] );
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
        a_vec = _mm256_set1_epi8( a );
        b_vec = tbl32_gf16_inv( a_vec );
        _mm256_storeu_si256( (__m256i *) v32, b_vec );

        if ( b != v32[0] ) {
            printf("a^-1 : %x ^-1->  %x : %x\n", a, b, v32[0] );
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
            a_vec = _mm256_set1_epi8( a );
            b_vec = _mm256_set1_epi8( b );
            c_vec = tbl32_gf16_mul( a_vec, b_vec );
            _mm256_storeu_si256( (__m256i *) v32, c_vec );

            if ( c != v32[0] ) {
                printf("axb : %x x %x ->  %x : %x\n", a, b, c, v32[0] );
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
        a_vec = _mm256_set1_epi8( a );
        b_vec = tbl32_gf256_squ( a_vec );
        _mm256_storeu_si256( (__m256i *) v32, b_vec );

        if ( b != v32[0] ) {
            printf("a^2 : %x ^2->  %x : %x\n", a, b, v32[0] );
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

    #if 0
    // function removed
    printf("testing inverse: ");
    for (unsigned i = 0; i < 256; i++) {
        a = i;
        b = gf256_inv( a );
        a_vec = _mm256_set1_epi8( a );
        b_vec = tbl32_gf256_inv( a_vec );
        _mm256_storeu_si256( (__m256i *) v32, b_vec );

        if ( b != v32[0] ) {
            printf("a^-1 : %x ^-1->  %x : %x\n", a, b, v32[0] );
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
    #endif

    printf("testing multiplication: ");
    for (unsigned i = 0; i < 256; i++) {
        for (unsigned j = 0; j < 256; j++) {
            a = i;
            b = j;
            c = gf256_mul( a, b );
            a_vec = _mm256_set1_epi8( a );
            //b_vec = _mm256_set1_epi8( b );
            c_vec = tbl32_gf256_mul_const( b, a_vec );
            _mm256_storeu_si256( (__m256i *) v32, c_vec );

            if ( c != v32[0] ) {
                printf("axb : %x x %x ->  %x : %x\n", a, b, c, v32[0] );
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


#endif   // !defined(_BLAS_AVX2_)
