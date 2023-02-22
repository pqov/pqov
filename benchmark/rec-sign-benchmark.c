
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"

#include "benchmark.h"


#define TEST_GENKEY 100
#define TEST_RUN 1000


uint64_t rec_keygen[TEST_GENKEY];
uint64_t rec_sign[TEST_RUN];
uint64_t rec_open[TEST_RUN];

int main(void) {
    #if defined(_MAC_OS_)&&defined(_M1CYCLES_)
    // rdtsc() in m1cycles.c needs initialization.
    struct benchmark bm0;
    bm_init(&bm0);
    #endif
    uint64_t t0, t1, t2;

    char msg[256];

    printf("| %s |", CRYPTO_ALGNAME );

    unsigned char sm[256 + CRYPTO_BYTES];
    unsigned char m[256];
    for (unsigned i = 0; i < 256; i++) {
        m[i] = i;
    }
    unsigned long long mlen = 256;
    unsigned long long smlen;

    unsigned char *pk = (unsigned char *)malloc( CRYPTO_PUBLICKEYBYTES );
    unsigned char *sk = (unsigned char *)malloc( CRYPTO_SECRETKEYBYTES );

    for (unsigned i = 0; i < TEST_GENKEY; i++) {
        int r;
        t0 = rdtsc();
        r = crypto_sign_keypair( pk, sk);
        t1 = rdtsc();
        rec_keygen[i] = t1 - t0;
        if ( 0 != r ) {
            printf("generating key return %d.\n", r);
            return -1;
        }
    }
    report(msg, 256, rec_keygen, TEST_GENKEY);
    printf(" %s |", msg );

    for (unsigned i = 0; i < TEST_RUN; i++) {
        for (unsigned j = 3; j < 256; j++) {
            m[j] = (i * j) & 0xff;
        }
        int r1, r2;
        t0 = rdtsc();
        r1 = crypto_sign( sm, &smlen, m, mlen, sk );
        t1 = rdtsc();
        r2 = crypto_sign_open( m, &mlen, sm, smlen, pk );
        t2 = rdtsc();

        rec_sign[i] = t1 - t0;
        rec_open[i] = t2 - t1;
        if ( 0 != r1 ) {
            printf("crypto_sign() return %d.\n", r1);
            return -1;
        }
        if ( 0 != r2 ) {
            printf("crypto_sign_open() return %d.\n", r2);
            return -1;
        }
    }
    report(msg, 256, rec_sign, TEST_RUN);
    printf(" %s |", msg );

    report(msg, 256, rec_open, TEST_RUN);
    printf(" %s |\n", msg );

    free( pk );
    free( sk );

    return 0;
}

