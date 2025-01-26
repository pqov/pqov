#ifndef _NEON_AESFFS_H_
#define _NEON_AESFFS_H_

#include "stdint.h"

/**
 * @brief AES-128 CTR with 8 consecutive counters.
 *
 * This function computes 8 AES-128 counter modes with 8 consective counters.
 *
 * @param[out] ctext0 8 AES-128 outputs, expected to be 8x16 bytes.
 * @param[in] iv Initial vectors for AES-128 CTR. 16 bytes.
 * @param[in] ctr The counter number. Output cipher texts will be with counters ctr+0, ctr+1, ... ctr+7.
 * @param[in] rkeys Round keys of AES-128, 88 bytes.
 */
void neon_aes128ctrx8_encrypt_ffs(unsigned char *ctext0, const uint8_t *iv, uint32_t ctr, const uint32_t *rkeys);

/**
 * @brief 4ROUNDS AES-128 CTR with 8 consecutive counters.
 *
 * This function computes 8 4ROUNDS AES-128 counter modes with 8 consective counters.
 *
 * @param[out] ctext0 8 AES-128 outputs, expected to be 8x16 bytes.
 * @param[in] iv Initial vectors for AES-128 CTR. 16 bytes.
 * @param[in] ctr The counter number. Output cipher texts will be with counters ctr+0, ctr+1, ... ctr+7.
 * @param[in] rkeys Round keys of 4ROUNDS AES-128, 40 bytes.
 */
void neon_aes128ctrx8_4r_encrypt_ffs(unsigned char *ctext0, const uint8_t *iv, uint32_t ctr, const uint32_t *rkeys);


#endif  // _NEON_AESFFS_H_
