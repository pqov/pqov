
#include <stdio.h>

#include "gf16.h"

#include "utils.h"

#include "string.h"

#include "gf16.h"


#include "config.h"

#if !defined(_BLAS_NEON_)

int main(void) {
    printf("ERROR -- No defining _BLAS_NEON_ in config.h\n\n");
    return -1;
}

#else   // !defined(_BLAS_NEON_)



#include "gf16_neon.h"


//#define _BM_

#ifdef _BM_
#include "benchmark.h"
#else
#define BENCHMARK(a,b)  do { b } while(0)
#endif


void gf16v_mul_wrap( uint8_t *c, const uint8_t *a, uint8_t b ) {
    uint8x16_t aa = vld1q_u8( a );

    uint8x16_t cc = gf16v_mul_neon( aa, b );

    vst1q_u8( c, cc );
}

static
uint8_t gf256_mul_neon( uint8_t a, uint8_t b ) {
    uint8x16_t aa = vdupq_n_u8( a );
    uint8x16_t cc = gf256v_mul_neon( aa, b );

    return vgetq_lane_u8( cc, 0 );
}


int main(void) {
    #ifdef _BM_
    struct benchmark bm1;
    struct benchmark bm2;
    struct benchmark bm3;
    bm_init( &bm1 );
    bm_init( &bm2 );
    bm_init( &bm3 );
    #endif

    printf("====== unit test ======\n");
    printf("Testing  gf_ele * gf_ele^-1 -> 1\n\n" );


    printf("\n\n============= GF(16) ==============\n");

    uint8_t a, b, c = 0;
    uint8_t a_vec[16], c_vec[16];

    a = 0;
    b = gf16_inv_neon(a);
    a_vec[0] = a;
    a_vec[1] = a << 4;

    gf16v_mul_wrap( c_vec, a_vec, b );

    if ( 0 != c_vec[0] || 0 != (c_vec[1] >> 4) ) {
        printf("a:%x ==^-1==> b:%x, axb->%x, %x\n", a, b, c_vec[0], c_vec[1] );
        printf("test failed.\n");
        return -1;
    }

    for (unsigned i = 1; i < 16; i++) {
        a = i;
        BENCHMARK( bm1, {
            for (int j = 0; j < 1000; j++)
                c = gf16_inv(a);
        } );
        BENCHMARK( bm2, {
            for (int j = 0; j < 1000; j++)
                b = gf16_inv_neon(a);
        } );
        a_vec[0] = a;
        a_vec[1] = a << 4;
        BENCHMARK( bm3, {
            for (int j = 0; j < 1000; j++)
                gf16v_mul_wrap( c_vec, a_vec, b );
        } );

        if ( 1 != c_vec[0] || 1 != (c_vec[1] >> 4) ) {
            printf("a:%x ==^-1==> b:%x, axb->%x, %x\n", a, b, c_vec[0], c_vec[1] );
            printf("test failed.\n");
            return -1;
        }
    }


    printf("\n\n============= GF(256) ==============\n");

    a = 0;
    b = gf256_inv(a);
    c = gf256_mul_neon(a, b);

    if ( 0 != c ) {
        printf("a:%x ==^-1==> b:%x, axb->%x\n", a, b, c );
        printf("test failed.\n");
        return -1;
    }

    for (unsigned i = 1; i < 256; i++) {
        a = i;

        b = gf256_inv(a);
        c = gf256_mul_neon(a, b);

        if ( 1 != c ) {
            printf("a:%x ==^-1==> b:%x, axb->%x\n", a, b, c );
            printf("test failed.\n");
            return -1;
        }
    }

    #ifdef _BM_
    char msg[256];
    bm_dump( msg, 256, &bm1 );
    printf("gf16 inv ref: %s\n", msg );
    bm_dump( msg, 256, &bm2 );
    printf("gf16 inv neon: %s\n", msg );
    bm_dump( msg, 256, &bm3 );
    printf("gf16v mul neon: %s\n", msg );
    #endif

    printf("all tests passed.\n");

    return 0;
}

#endif   // !defined(_BLAS_NEON_)

