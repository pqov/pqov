
#include <stdio.h>

#include "gf16.h"

#include "utils.h"

#include "string.h"


#include "config.h"

#if ! (defined(_BLAS_AVX2_)&&defined(_BLAS_GFNI_))

int main(void) {
    printf("ERROR -- No defining _BLAS_AVX2_ and _BLAS_GFNI_ in config.h\n\n");
    return -1;
}


#else   // ! (defined(_BLAS_AVX2_)&&defined(_BLAS_GFNI_))


#include "gf16_gfni.h"



uint8_t cmp_eq( const uint8_t * v0 , const uint8_t * v1 , unsigned len )
{
    uint8_t r = 0;
    for(unsigned i=0;i<len;i++) r |= v0[i]^v1[i];
    return 0==r;
}



int main() {

    printf("====== unit test ======\n");
    printf("Testing  consistency of basic arithmetic between ref and gfni.\n\n" );


    printf("\n\n============= GF(16) ==============\n");

    uint8_t m0[32*16];
    uint8_t m1[32*16];


    int fail = 0;


    printf("testing multiplication: ");
    for(unsigned i=0;i<16;i++) {
        for(unsigned j=0;j<16;j++) {
            m0[i*32+j]    = gf16_mul( j , i );
            m0[i*32+j+16] = gf16_mul( j , i )<<4;
        }

        for(unsigned j=0;j<16;j++) { m1[i*32+j] = j; m1[i*32+j+16] = j<<4; }
        __m256i m1v  = _mm256_loadu_si256( (__m256i*) &m1[i*32] );
        __m256i m1vx = gf16v_mul_gfni( m1v , i );
        _mm256_storeu_si256( (__m256i*) &m1[i*32] , m1vx );

        if ( ! cmp_eq(&m0[i*32],&m1[i*32],32) ) {
            printf( "!eq while vec x %x:\n" , i );
            printf( "ref :" ); for(int k=0;k<32;k++) { printf( "%x ", m0[i*32+k] ); } printf("\n");
            printf( "gfni:" ); for(int k=0;k<32;k++) { printf( "%x ", m1[i*32+k] ); } printf("\n");
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

    return 0;
}


#endif   // ! (defined(_BLAS_AVX2_)&&defined(_BLAS_GFNI_))
