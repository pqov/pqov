// SPDX-License-Identifier: CC0 OR Apache-2.0
/// @file gf16_tabs.h
/// @brief Defining the constants for performing GF arithmetics.
///

#ifndef _GF16_TABS_NEON_H_
#define _GF16_TABS_NEON_H_

#include <stdint.h>

//IF_CRYPTO_CORE:define CRYPTO_NAMESPACE

#ifdef  __cplusplus
extern  "C" {
#endif

extern const unsigned char __0_f[];
extern const unsigned char __gf16_reduce[];

extern const unsigned char __gf16_inv[];
extern const unsigned char __gf16_squ[];
extern const unsigned char __gf16_mulbase[];
extern const unsigned char __gf16_mul[];

extern const unsigned char __gf256_bit8_11_reduce[];
extern const unsigned char __gf256_bit12_15_reduce[];
extern const unsigned char __gf256_squ[];
extern const unsigned char __gf256_mulbase[];
extern const unsigned char __gf256_mul[];


#ifdef  __cplusplus
}
#endif



#endif // _GF16_TABS_NEON_H_
