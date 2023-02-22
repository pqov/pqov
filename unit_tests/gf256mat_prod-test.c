
#include <stdio.h>



#include "blas.h"
#include "blas_comm.h"
#include "blas_matrix.h"

#include "blas_matrix_ref.h"

#include "utils_prng.h"

#define prng_t prng_publicinputs_t
#define prng_set prng_set_publicinputs
#define prng_gen prng_gen_publicinputs

#include "utils.h"

#include "string.h"


#include "params.h"

#if defined(_USE_GF16)

int main(void) {
    printf( "\nERROR -- Macro _USE_GF16 defined in params.h\n\n" );
    return -1;
}

#else // defined(_USE_GF16)




#define MAX_LEN  96
#define TEST_RUN 100

int main(void) {

    printf("====== unit test ======\n");
    printf("matrix[%dx%d]-vector product test\n\n", MAX_LEN, MAX_LEN );


    printf("\n\n============= setup PRNG ==============\n");

    prng_t _prng;
    prng_t *prng0 = &_prng;
    uint8_t prng_seed[32] = {0};
    prng_set( prng0, prng_seed );

    printf("\n\n============= random matrix generation ==============\n");

    uint8_t matA[ MAX_LEN * MAX_LEN ];
    uint8_t vec_b[ MAX_LEN ];
    uint8_t vec_c0[ MAX_LEN ];
    uint8_t vec_c1[ MAX_LEN ];


    int fail = 0;

    unsigned lens[] = { 22, 32, 36, 44, 48, 64, 72, 96 };
    for (unsigned i = 0; i < sizeof(lens) / sizeof(unsigned); i++) {
        printf("matrix[%dx%d]-vector product test\n", lens[i], lens[i] );


        for (int j = 0; j < TEST_RUN; j++) {
            prng_gen( prng0, matA, lens[i]*lens[i] );
            prng_gen( prng0, vec_b, lens[i] );


            gf256mat_prod_ref( vec_c0, matA, lens[i], lens[i], vec_b );
            gf256mat_prod( vec_c1, matA, lens[i], lens[i], vec_b );

            gf256v_add( vec_c0, vec_c1, lens[i] );

            if ( !gf256v_is_zero( vec_c0, lens[i] ) ) {
                gf256v_add( vec_c0, vec_c1, lens[i] );

                printf("in-correct:\n");

                byte_fdump( stdout, "vec_b : ", vec_b, lens[i] );
                printf("\n");

                byte_fdump( stdout, "vec_c0: ", vec_c0, lens[i] );
                printf("\n");
                byte_fdump( stdout, "vec_c1: ", vec_c1, lens[i] );
                printf("\n");

                fail = 1;
                break;
            }
        }

        if ( fail ) {
            break;
        }
    }
    printf((fail) ? "TEST FAIL.!\n" : "test PASS.\n" );


    return fail;
}


#endif // defined(_USE_GF16)
