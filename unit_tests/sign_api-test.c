
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"

//#include "benchmark.h"

#if defined(_VALGRIND_)
#define TEST_GENKEY 2
#define TEST_RUN 5
#else
#define TEST_GENKEY 50
#define TEST_RUN 500
#endif

int main(void) {
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

    int ret = 0;

    printf("===========  test crypto_sign_keypair(), crypto_sign(), and crypto_sign_open()  ================\n\n");
    for (unsigned i = 0; i < TEST_RUN; i++) {
        if ( i < TEST_GENKEY ) {
            int r0;
            r0 = crypto_sign_keypair( pk, sk);
            if ( 0 != r0 ) {
                printf("generating key return %d.\n", r0);
                ret = -1;
                goto clean_exit;
            }
        }

        for (unsigned j = 3; j < 256; j++) {
            m[j] = (i * j) & 0xff;
        }
        int r1, r2;
        r1 = crypto_sign( sm, &smlen, m, mlen, sk );
        if ( 0 != r1 ) {
            printf("crypto_sign() return %d.\n", r1);
            ret = -1;
            goto clean_exit;
        }
        r2 = crypto_sign_open( m, &mlen, sm, smlen, pk );
        if ( 0 != r2 ) {
            printf("crypto_sign_open() return %d.\n", r2);
            ret = -1;
            goto clean_exit;
        }
    }
    printf("all (%d,%d) tests passed.\n\n", TEST_RUN, TEST_GENKEY );

clean_exit:
    free( pk );
    free( sk );
    return ret;
}

