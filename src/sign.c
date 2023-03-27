///  @file  sign.c
///  @brief the implementations for functions in api.h
///
///
#include <stdlib.h>
#include <string.h>

#include "params.h"
#include "ov_keypair.h"
#include "ov.h"

#include "api.h"

#include "utils_prng.h"

#if defined(_UTILS_SUPERCOP_)
#include "crypto_sign.h"
#endif


#if defined(_VALGRIND_)
#include "valgrind/memcheck.h"
#endif

int
crypto_sign_keypair(unsigned char *pk, unsigned char *sk) {
    unsigned char sk_seed[LEN_SKSEED];
    unsigned char pk_seed[LEN_PKSEED];
    randombytes( sk_seed, LEN_SKSEED );
    randombytes( pk_seed, LEN_PKSEED );

    #if defined(_VALGRIND_)
    VALGRIND_MAKE_MEM_UNDEFINED(sk_seed, LEN_SKSEED );  // mark secret data as undefined data
    #endif

    #if defined _OV_CLASSIC
    int r = generate_keypair( (pk_t *) pk, (sk_t *) sk, pk_seed, sk_seed );
    #elif defined _OV_PKC
    int r = generate_keypair_pkc( (cpk_t *) pk, (sk_t *) sk, pk_seed, sk_seed );
    #elif defined _OV_PKC_SKC
    int r = generate_keypair_pkc_skc( (cpk_t *) pk, (csk_t *) sk, pk_seed, sk_seed );
    #else
    error here
    #endif

    #if defined(_VALGRIND_)
    VALGRIND_MAKE_MEM_DEFINED(pk, OV_PUBLICKEYBYTES );  // mark return value as public data
    VALGRIND_MAKE_MEM_DEFINED(&r, sizeof(int) );  // mark return value as public data
    #endif
    return r;
}





int
#if defined(PQM4)
crypto_sign(unsigned char *sm, size_t *smlen, const unsigned char *m, size_t mlen, const unsigned char *sk)
#else
crypto_sign(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
#endif
{
    int r = -1;
    #if defined _OV_CLASSIC

    r = ov_sign( sm + mlen, (const sk_t *)sk, m, mlen );

    #elif defined _OV_PKC

    r = ov_sign( sm + mlen, (const sk_t *)sk, m, mlen );

    #elif defined _OV_PKC_SKC

    r = ov_expand_and_sign( sm + mlen, (const csk_t *)sk, m, mlen );

    #else
    error here
    #endif
    memmove( sm, m, mlen );
    smlen[0] = mlen + OV_SIGNATUREBYTES;

    return r;
}






int
#if defined(PQM4)
crypto_sign_open(unsigned char *m, size_t *mlen, const unsigned char *sm, size_t smlen, const unsigned char *pk)
#else
crypto_sign_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk)
#endif
{
    if ( OV_SIGNATUREBYTES > smlen ) {
        return -1;
    }
    unsigned long mesg_len = smlen - OV_SIGNATUREBYTES;
    int r = -1;

    #if defined _OV_CLASSIC

    r = ov_verify( sm, mesg_len, sm + mesg_len, (const pk_t *)pk );

    #elif defined _OV_PKC

    r = ov_expand_and_verify( sm, mesg_len, sm + mesg_len, (const cpk_t *)pk );

    #elif defined _OV_PKC_SKC

    r = ov_expand_and_verify( sm, mesg_len, sm + mesg_len, (const cpk_t *)pk );

    #else
    error here
    #endif

    memmove( m, sm, mesg_len );
    mlen[0] = mesg_len;

    return r;
}

