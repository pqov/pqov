
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "benchmark.h"


#include "blas_avx2.h"
#include "blas_avx2_gfni.h"


#define N_LARGE    (512)
#define N_SMALL    (31)
#define MULTAB_BYTE (32)


uint8_t largeMULTAB0[N_LARGE*MULTAB_BYTE] __attribute__((aligned(32)));
uint8_t largeMULTAB1[N_LARGE*MULTAB_BYTE] __attribute__((aligned(32)));

uint8_t smallMULTAB0[N_SMALL*MULTAB_BYTE] __attribute__((aligned(32)));
uint8_t smallMULTAB1[N_SMALL*MULTAB_BYTE] __attribute__((aligned(32)));


static
uint8_t check_neq( const uint8_t * tab0 , const uint8_t * tab1 , unsigned size )
{
    uint8_t r = 0;
    for(unsigned i=0;i<size;i++) r |= (tab0[i]^tab1[i]);
    return r;
}


#define TEST_RUN  (1000)


int main(void) {
    struct benchmark bm1, bm2, bm3, bm4;
    bm_init(&bm1);
    bm_init(&bm2);
    bm_init(&bm3);
    bm_init(&bm4);


    char msg[256];
    for (unsigned i = 0; i < 256; i++) {
        msg[i] = i;
    }

    printf("Benchmark generating GF(16) multab with avx2 and gfni:\n");
    uint8_t vec[N_LARGE/2];
    for (unsigned i=0;i<sizeof(vec);i++) {
        vec[i] = i*13;
    }

    printf("===========  benchmark generating LARGE (%d) MUL tables   ================\n\n", N_LARGE );
    for (unsigned i = 0; i < TEST_RUN; i++) {
        BENCHMARK(bm1, {
            gf16v_generate_multabs_avx2( largeMULTAB0, vec, N_LARGE );
        });
        BENCHMARK(bm2, {
            gf16v_generate_multabs_gfni( largeMULTAB1, vec, N_LARGE );
        });

        if ( check_neq( largeMULTAB0, largeMULTAB1 , sizeof(largeMULTAB0)) ) {
            printf("!!! Tables NEQ [%d].\n", i);
            return -1;
        }
    }
    bm_dump(msg, 256, &bm1);
    printf("benchmark:  LARGE tables with avx2: %s\n\n", msg );
    bm_dump(msg, 256, &bm2);
    printf("benchmark:  LARGE tables with gfni: %s\n\n", msg );

    printf("===========  benchmark generating SMALL (%d) MUL tables   ================\n\n", N_SMALL );
    for (unsigned i = 0; i < TEST_RUN; i++) {
        BENCHMARK(bm3, {
            gf16v_generate_multabs_avx2( smallMULTAB0, vec, N_SMALL );
        });
        BENCHMARK(bm4, {
            gf16v_generate_multabs_gfni( smallMULTAB1, vec, N_SMALL );
        });

        if ( check_neq( smallMULTAB0, smallMULTAB1 , sizeof(smallMULTAB0)) ) {
            printf("!!! Tables NEQ [%d].\n", i);
            return -1;
        }
    }
    bm_dump(msg, 256, &bm3);
    printf("benchmark:  SMALL tables with avx2: %s\n\n", msg );
    bm_dump(msg, 256, &bm4);
    printf("benchmark:  SMALL tables with gfni: %s\n\n", msg );


    return 0;
}

