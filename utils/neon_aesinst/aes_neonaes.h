#ifndef _AES_NEONAES_H_
#define _AES_NEONAES_H_


#include "stdint.h"


void aes256_keyexp_neonaes( uint8_t *roundkeys, const uint8_t *aes256_key);   // roundkeys: 15*16 bytes

void aes256_enc_neonaes( uint8_t *ct, const uint8_t *pk, const uint8_t *aes256_rk );



void aes128_keyexp_neonaes( uint8_t *roundkeys, const uint8_t *aes128_key);   // roundkeys: 11*16 bytes

void aes128_enc_neonaes( uint8_t *ct, const uint8_t *pk, const uint8_t *aes128_rk );


void aes128_4r_keyexp_neonaes( uint8_t *roundkeys, const uint8_t *aes128_key);   // roundkeys: 5*16 bytes

void aes128_4r_enc_neonaes( uint8_t *ct, const uint8_t *pk, const uint8_t *aes128_rk );



void aes256ctrx4_enc_neonaes( uint8_t *ct, const uint8_t *nonce, uint32_t ctr, const uint8_t *aes256_rk );

void aes128ctrx4_enc_neonaes( uint8_t *ct, const uint8_t *nonce, uint32_t ctr, const uint8_t *aes128_rk );

void aes128ctrx4_4r_enc_neonaes( uint8_t *ct, const uint8_t *nonce, uint32_t ctr, const uint8_t *aes128_rk );





#endif  // ndef _AES_H_
