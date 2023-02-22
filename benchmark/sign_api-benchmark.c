
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"

#include "benchmark.h"


#define TEST_GENKEY 50
#define TEST_RUN 500


int main(void) {
    struct benchmark bm1, bm2, bm3;
    bm_init(&bm1);
    bm_init(&bm2);
    bm_init(&bm3);

    char msg[256];

    for (unsigned i = 0; i < 256; i++) {
        msg[i] = i;
    }

    printf("%s\n", OV_ALGNAME );
    printf("sk size: %d\n", CRYPTO_SECRETKEYBYTES );
    printf("pk size: %d\n", CRYPTO_PUBLICKEYBYTES );
    printf("signature overhead: %d\n\n", CRYPTO_BYTES );

    unsigned char sm[256 + CRYPTO_BYTES];
    unsigned char m[256];
    for (unsigned i = 0; i < 256; i++) {
        m[i] = i;
    }
    unsigned long long mlen = 256;
    unsigned long long smlen;

    unsigned char *pk = (unsigned char *)malloc( CRYPTO_PUBLICKEYBYTES );
    unsigned char *sk = (unsigned char *)malloc( CRYPTO_SECRETKEYBYTES );

    printf("===========  benchmark crypto_sign_keypair()  ================\n\n");
    for (unsigned i = 0; i < TEST_GENKEY; i++) {
        int r;
        BENCHMARK(bm1, {
            r = crypto_sign_keypair( pk, sk);
        });
        if ( 0 != r ) {
            printf("generating key return %d.\n", r);
            return -1;
        }
    }
    bm_dump(msg, 256, &bm1);
    printf("benchmark:  crypto_sign_keypair(): %s\n\n", msg );


    printf("===========  benchmark crypto_sign() and crypto_sign_open()  ================\n\n");
    for (unsigned i = 0; i < TEST_RUN; i++) {
        for (unsigned j = 3; j < 256; j++) {
            m[j] = (i * j) & 0xff;
        }
        int r1, r2;
        BENCHMARK(bm2, {
            r1 = crypto_sign( sm, &smlen, m, mlen, sk );
        });
        BENCHMARK(bm3, {
            r2 = crypto_sign_open( m, &mlen, sm, smlen, pk );
        });
        if ( 0 != r1 ) {
            printf("crypto_sign() return %d.\n", r1);
            return -1;
        }
        if ( 0 != r2 ) {
            printf("crypto_sign_open() return %d.\n", r2);
            return -1;
        }
    }
    bm_dump(msg, 256, &bm2);
    printf("benchmark:  crypto_sign(): %s\n\n", msg );

    bm_dump(msg, 256, &bm3);
    printf("benchmark:  crypto_sign_open(): %s\n\n", msg );

    free( pk );
    free( sk );

    return 0;
}

