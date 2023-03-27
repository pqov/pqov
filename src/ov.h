/// @file ov.h
/// @brief APIs for ov.
///

#ifndef _OV_H_
#define _OV_H_

#include "params.h"
#include "ov_keypair.h"

#include <stdint.h>
#include <stdio.h>  // for size_t

#ifdef  __cplusplus
extern  "C" {
#endif



////////////////////////  Key pair generation  ///////////////////////////////////


/// Classic OV  /////////////////////////////////

///
/// @brief Key pair generation for classic ov.
///
/// @param[out] pk        - the public key.
/// @param[out] sk        - the secret key.
/// @param[in]  pk_seed   - seed for generating P1,P2 of the public key.
/// @param[in]  sk_seed   - seed for generating the secret key.
/// @return 0 for success. -1 otherwise.
///
int generate_keypair( pk_t *pk, sk_t *sk, const unsigned char *pk_seed, const unsigned char *sk_seed);



////////////////////////////////////

///
/// @brief Secret key Generation for classic ov.
///
/// @param[out] sk        - the secret key.
/// @param[in]  pk_seed   - seed for generating P1,P2 of the public key.
/// @param[in]  sk_seed   - seed for generating the secret key.
///
void generate_secretkey( sk_t *sk, const unsigned char *pk_seed, const unsigned char *sk_seed);

///
/// @brief Convert secret key to public key for classic ov.
///
/// @param[out] pk        - the public key.
/// @param[in]  sk        - the secret key.
/// @param[in]  sk_seed   - seed for generating the secret key.
/// @return 0 for success. -1 otherwise.
///
int sk_to_pk( pk_t *pk, const sk_t *sk, const unsigned char *pk_seed );



///
/// @brief Generate key pair for pkc
///
/// @param[out] pk        - the compressed public key.
/// @param[out] sk        - the secret key.
/// @param[in]  pk_seed   - seed for generating parts of public key.
/// @param[in]  sk_seed   - seed for generating secret key.
/// @return 0 for success. -1 otherwise.
///
int generate_keypair_pkc( cpk_t *pk, sk_t *sk, const unsigned char *pk_seed, const unsigned char *sk_seed );

///
/// @brief Generate key pair for pkc+skc
///
/// @param[out] pk        - the compressed public key.
/// @param[out] sk        - the compressed secret key.
/// @param[in]  pk_seed   - seed for generating parts of the public key.
/// @param[in]  sk_seed   - seed for generating the secret key.
/// @return 0 for success. -1 otherwise.
///
int generate_keypair_pkc_skc( cpk_t *pk, csk_t *sk, const unsigned char *pk_seed, const unsigned char *sk_seed );





/// Key conversion : compressed key pair -> classic key pair //////////////////////////////


///
/// @brief converting formats of public keys : from compressed to classic public key
///
/// @param[out] pk       - the classic public key.
/// @param[in]  cpk      - the cycli  public key.
///
int expand_pk( pk_t *pk, const cpk_t *cpk );

///
/// @brief converting formats of public keys : from compressed to classic public key
///
/// @param[out] pk       - the classic public key.
/// @param[in]  cpk      - the cycli  public key.
/// @param[in]  predicate - if 0==predicate[i], the public key corresponding to xi will not be generated.
///
int expand_pk_predicate( pk_t *pk, const cpk_t *cpk, const unsigned char *predicate);


///
/// @brief Generate secret key
///
/// @param[out] sk        - the secret key.
/// @param[in]  pk_seed   - seed for generating parts of the public key.
/// @param[in]  sk_seed   - seed for generating the secret key.
/// @return 0 for success. -1 otherwise.
///
int expand_sk( sk_t *sk, const unsigned char *pk_seed, const unsigned char *sk_seed );


////////////////////////  Public map  ///////////////////////////////////


///
/// @brief Public-key evaluattion
///
/// @param[out] z         - results of the evluation of public polynomials at the w.
/// @param[in]  pk        - the classic public key.
/// @param[in]  w         - the input vector w.
///
void ov_publicmap( unsigned char *z, const unsigned char *pk, const unsigned char *w );



///
/// @brief Public-key evaluattion
///
/// @param[out] z         - results of the evluation of public polynomials at the w.
/// @param[in]  pk        - the compressed public key.
/// @param[in]  w         - the input vector w.
///
void ov_publicmap_pkc( unsigned char *z, const cpk_t *pk, const unsigned char *w );








///////////////////////////// Sign and Verify ////////////////////////////////



///
/// @brief Signing function for classic secret key.
///
/// @param[out] signature - the signature.
/// @param[in]  sk        - the secret key.
/// @param[in]  message   - the message to be signed.
/// @param[in]  mlen      - the length of the message.
/// @return 0 for success. -1 otherwise.
///
int ov_sign( uint8_t *signature, const sk_t *sk, const uint8_t *message, size_t mlen );

///
/// @brief Verifying function.
///
/// @param[in]  message   - the message.
/// @param[in]  mlen      - the length of the message.
/// @param[in]  signature - the signature.
/// @param[in]  pk        - the public key.
/// @return 0 for successful verified. -1 for failed verification.
///
int ov_verify( const uint8_t *message, size_t mlen, const uint8_t *signature, const pk_t *pk );





///
/// @brief Signing function for compressed secret key.
///
/// @param[out] signature - the signature.
/// @param[in]  sk        - the compressed secret key.
/// @param[in]  message   - the message to be signed.
/// @param[in]  mlen      - the length of the message.
/// @return 0 for success. -1 otherwise.
///
int ov_expand_and_sign( uint8_t *signature, const csk_t *sk, const uint8_t *message, size_t mlen );

///
/// @brief Verifying function for compressed public keys.
///
/// @param[in]  message   - the message to be signed.
/// @param[in]  mlen      - the length of the message.
/// @param[in]  signature - the signature.
/// @param[in]  pk        - the public key of cyclic OV.
/// @return 0 for successful verified. -1 for failed verification.
///
int ov_expand_and_verify( const uint8_t *message, size_t mlen, const uint8_t *signature, const cpk_t *pk );






#ifdef  __cplusplus
}
#endif


#endif // _OV_H_
