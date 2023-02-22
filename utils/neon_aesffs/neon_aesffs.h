#ifndef _NEON_AESFFS_H_
#define _NEON_AESFFS_H_

#include "stdint.h"

//void neon_aes256ctrx8_encrypt_ffs(unsigned char ctext0[16*8] , const uint8_t * iv, uint32_t ctr , const uint32_t rkeys[120]);

void neon_aes128ctrx8_encrypt_ffs(unsigned char ctext0[16 * 8], const uint8_t *iv, uint32_t ctr, const uint32_t rkeys[88]);

void neon_aes128ctrx8_4r_encrypt_ffs(unsigned char ctext0[16 * 8], const uint8_t *iv, uint32_t ctr, const uint32_t rkeys[40]);


#endif  // _NEON_AESFFS_H_
