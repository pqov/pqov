/// @file ov_keypair_computation.h
/// @brief Functions for calculating pk/sk while generating keys.
///
/// Defining an internal structure of public key.
/// Functions for calculating pk/sk for key generation.
///

#ifndef _ov_KEYPAIR_COMP_H_
#define _ov_KEYPAIR_COMP_H_

#include "ov_keypair.h"

//IF_CRYPTO_CORE:define CRYPTO_NAMESPACE

#ifdef  __cplusplus
extern  "C" {
#endif




///
/// @brief Computing public key from secret key, classic ov
///
/// @param[out] Qs       - the public key: l1_Q1, l1_Q2, l1_Q5
/// @param[in]  Fs       - parts of the secret key: l1_F1, l1_F2
/// @param[in]  Ts       - parts of the secret key: T1
///
void calculate_Q_from_F( pk_t *Qs, const sk_t *Fs, const sk_t *Ts );

///
/// @brief Computing parts of the sk from parts of pk and sk
///
/// @param[out] Fs       - secret key
///
void ov_pkc_calculate_F_from_Q( sk_t *Fs);

///
/// @brief Computing parts of the pk from the secret key
///
/// @param[out] Qs       - parts of the pk: l1_Q5
/// @param[in]  Fs       - parts of the sk: l1_F1, l1_F2
/// @param[in]  Ts       - parts of the sk: T1
///
void ov_pkc_calculate_Q_from_F( cpk_t *Qs, const sk_t *Fs, const sk_t *Ts );



///
/// @brief Computing sk F2 and pk P2 from P1, P2, and sk O
///
/// @param[out] F2       - F2 of the sk
/// @param[out] P3       - P3 of the pk
/// @param[in]  P1       - P1 of the pk
/// @param[in]  P2       - P2 of the pk
/// @param[in]  sk       - O of the sk
///
void calculate_F2_P3( unsigned char *F2, unsigned char *P3, const unsigned char *P1, const unsigned char *P2, const unsigned char *sk );

///
/// @brief Computing sk F2 from P1, P2, and sk O
///
/// @param[out] F2       - F2 of the sk
/// @param[in]  P1       - P1 of the pk
/// @param[in]  P2       - P2 of the pk
/// @param[in]  sk       - O of the sk
///
void calculate_F2( unsigned char *F2, const unsigned char *P1, const unsigned char *P2, const unsigned char *sk );

///
/// @brief Computing P3 of the pk from P1, P2, and sk O
///
/// @param[out] P3       - P3 of the pk
/// @param[in]  P1       - P1 of the pk
/// @param[in]  P2       - P2 of the pk
/// @param[in]  sk       - O of the sk
///
void calculate_P3( unsigned char *P3, const unsigned char *P1, const unsigned char *P2, const unsigned char *sk );




#ifdef  __cplusplus
}
#endif

#endif  // _ov_KEYPAIR_COMP_H_

