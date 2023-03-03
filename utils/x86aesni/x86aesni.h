#ifndef _X86AESNI_H_
#define _X86AESNI_H_

#include "stdint.h"

#define _AES256_ROUNDKEY_BYTE  (15*16)

#define _AES128_ROUNDKEY_BYTE  (11*16)

#define _AES128_4R_ROUNDKEY_BYTE  (5*16)


void AES256_Key_Expansion (unsigned char *key, const unsigned char *userkey);

void AES256_CTR_Encrypt ( unsigned char *out, unsigned long n_16B, const unsigned char *key, const unsigned char nonce[16], uint32_t ctr );


void AES128_Key_Expansion (unsigned char *key, const unsigned char *userkey);

void AES128_CTR_Encrypt ( unsigned char *out, unsigned long n_16B, const unsigned char *key, const unsigned char nonce[16], uint32_t ctr );


void AES128_4R_Key_Expansion (unsigned char *key, const unsigned char *userkey);

void AES128_4R_CTR_Encrypt ( unsigned char *out, unsigned long n_16B, const unsigned char *key, const unsigned char nonce[16], uint32_t ctr );


#endif  // _X86AESNI_H_
