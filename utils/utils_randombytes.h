/// @file utils_randombytes.h
/// @brief wrappers for randombytes().
///
///

#ifndef _UTILS_RANDOMBYTES_H_
#define _UTILS_RANDOMBYTES_H_


#ifdef  __cplusplus
extern  "C" {
#endif


#include "config.h"


#if defined(_UTILS_SUPERCOP_)||defined(_UTILS_PQM4_)

#include "randombytes.h"

#elif defined(_NIST_KAT_) && defined(_UTILS_OPENSSL_)

#include "rng.h"

#else

void randombytes(unsigned char *x, unsigned long long xlen);

#endif


#ifdef  __cplusplus
}
#endif

#endif // _UTILS_RANDOMBYTES_H_


