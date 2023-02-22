
#include "stdio.h"
#include "stdint.h"

#include "utils.h"

#include "utils_hash.h"


static
int vec_eq( const uint8_t *v0, const uint8_t *v1, unsigned len ) {
    uint8_t r = 0;
    for (unsigned i = 0; i < len; i++) {
        r |= v0[i] ^ v1[i];
    }
    return r == 0;
}

static
int test_ctx_copy( const uint8_t *in, unsigned inlen ) {
    hash_ctx ctx0, ctx1, ctx2;

    hash_init( &ctx0 );

    hash_update( &ctx0, in, inlen );

    hash_ctx_copy( &ctx1, &ctx0 );
    hash_ctx_copy( &ctx2, &ctx0 );

    uint8_t out0[32];
    uint8_t out1[32];
    uint8_t out2[32];

    hash_final_digest( out0, 8, &ctx0 );
    byte_fdump(stdout, "hash[8]: ", out0, 8 );
    puts("");

    hash_final_digest( out1, 16, &ctx1 );
    byte_fdump(stdout, "hash[16]:", out1, 16 );
    puts("");

    hash_final_digest( out2, 24, &ctx2 );
    byte_fdump(stdout, "hash[24]:", out2, 24 );
    puts("");

    int eq = vec_eq(out1, out2, 16 );
    printf("hash[16] eq? %d\n", eq );

    return (eq) ? 0 : 1;
}

static
int test_hash_update() {

    uint8_t out0[8];
    uint8_t out1[8];
    int outlen = 8;


    uint8_t mesg[17] = "0123456789abcdef";

    hash_ctx ctx0;
    hash_ctx ctx1;

    hash_init( &ctx0 );
    hash_init( &ctx1 );

    hash_update( &ctx0, mesg, 8 );
    hash_update( &ctx0, mesg + 8, 8 );

    hash_update( &ctx1, mesg, 16 );

    hash_final_digest( out0, outlen, &ctx0 );
    byte_fdump(stdout, "hash1:", out0, outlen );
    puts("");

    hash_final_digest( out1, outlen, &ctx1 );
    byte_fdump(stdout, "hash:", out1, outlen );
    puts("");

    int eq = vec_eq(out0, out1, outlen);
    printf("eq? %d\n", eq );

    return (eq) ? 0 : 1;
}


int main(void) {

    int r = 0;
    r |= test_hash_update();

    uint8_t mesg[17] = "0123456789abcdef";
    r |= test_ctx_copy(mesg, 16);

    printf( (0 == r) ? "TEST PASSED\n" : "TEST FAILED\n" );
    return 0;
}
