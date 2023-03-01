/******************************************************************************
* C language implementations of the AES-128 and AES-256 key schedules to match
* the fixsliced representation. Note that those implementations rely on Look-Up
* Tables (LUT).
*
* See the paper at https://eprint.iacr.org/2020/1123.pdf for more details.
*
* @author   Alexandre Adomnicai, Nanyang Technological University, Singapore
*           alexandre.adomnicai@ntu.edu.sg
*
* @date     August 2020
******************************************************************************/
#include "aes128_4r_ffs.h"


#include <stdint.h>

#define ROR(x,y)                (((x) >> (y)) | ((x) << (32 - (y))))

#define SWAPMOVE(a, b, mask, n) do{                                                      \
        tmp = (b ^ (a >> n)) & mask;                                                    \
        b ^= tmp;                                                                                               \
        a ^= (tmp << n);                                                                                \
    } while(0)

#define LE_LOAD_32(x)                                                                           \
    ((((uint32_t)((x)[3])) << 24) |                                             \
     (((uint32_t)((x)[2])) << 16) |                                             \
     (((uint32_t)((x)[1])) << 8) |                                                      \
     ((uint32_t)((x)[0])))








/******************************************************************************
* Packs two 128-bit input blocs in0, in1 into the 256-bit internal state out
* where the bits are packed as follows:
* out[0] = b_24 b_56 b_88 b_120 || ... || b_0 b_32 b_64 b_96
* out[1] = b_25 b_57 b_89 b_121 || ... || b_1 b_33 b_65 b_97
* out[2] = b_26 b_58 b_90 b_122 || ... || b_2 b_34 b_66 b_98
* out[3] = b_27 b_59 b_91 b_123 || ... || b_3 b_35 b_67 b_99
* out[4] = b_28 b_60 b_92 b_124 || ... || b_4 b_36 b_68 b_100
* out[5] = b_29 b_61 b_93 b_125 || ... || b_5 b_37 b_69 b_101
* out[6] = b_30 b_62 b_94 b_126 || ... || b_6 b_38 b_70 b_102
* out[7] = b_31 b_63 b_95 b_127 || ... || b_7 b_39 b_71 b_103
******************************************************************************/
static void packing(uint32_t *out, const unsigned char *in0,
                    const unsigned char *in1) {
    uint32_t tmp;
    out[0] = LE_LOAD_32(in0);
    out[1] = LE_LOAD_32(in1);
    out[2] = LE_LOAD_32(in0 + 4);
    out[3] = LE_LOAD_32(in1 + 4);
    out[4] = LE_LOAD_32(in0 + 8);
    out[5] = LE_LOAD_32(in1 + 8);
    out[6] = LE_LOAD_32(in0 + 12);
    out[7] = LE_LOAD_32(in1 + 12);
    SWAPMOVE(out[1], out[0], 0x55555555, 1);
    SWAPMOVE(out[3], out[2], 0x55555555, 1);
    SWAPMOVE(out[5], out[4], 0x55555555, 1);
    SWAPMOVE(out[7], out[6], 0x55555555, 1);
    SWAPMOVE(out[2], out[0], 0x33333333, 2);
    SWAPMOVE(out[3], out[1], 0x33333333, 2);
    SWAPMOVE(out[6], out[4], 0x33333333, 2);
    SWAPMOVE(out[7], out[5], 0x33333333, 2);
    SWAPMOVE(out[4], out[0], 0x0f0f0f0f, 4);
    SWAPMOVE(out[5], out[1], 0x0f0f0f0f, 4);
    SWAPMOVE(out[6], out[2], 0x0f0f0f0f, 4);
    SWAPMOVE(out[7], out[3], 0x0f0f0f0f, 4);
}




/******************************************************************************
* LUT of the AES S-box.
******************************************************************************/
static unsigned char sbox_lut[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/******************************************************************************
* AES round constants.
******************************************************************************/
static unsigned char rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};


/******************************************************************************
* Pre-computes all the round keys for a given encryption key, according to the
* fully-fixsliced (ffs) representation.
* Note that the round keys also include the NOTs omitted in the S-box.
******************************************************************************/
void aes128_keyschedule_ffs_lut(uint32_t *rkeys_ffs, const unsigned char *key) {
    uint32_t t0, t1, t2, tmp;
    uint32_t rkeys[44];
    // key schedule in the classical representation
    rkeys[0] = LE_LOAD_32(key);
    rkeys[1] = LE_LOAD_32(key + 4);
    rkeys[2] = LE_LOAD_32(key + 8);
    rkeys[3] = LE_LOAD_32(key + 12);
    for (int i = 4; i < 44; i += 4) {
        rkeys[i] = rkeys[i - 4] ^ rcon[i / 4];
        rkeys[i] ^= ((uint32_t)sbox_lut[rkeys[i - 1] & 0xff] << 24);
        rkeys[i] ^= sbox_lut[(rkeys[i - 1] >> 8) & 0xff];
        rkeys[i] ^= (sbox_lut[rkeys[i - 1] >> 24] << 16);
        rkeys[i] ^= (sbox_lut[(rkeys[i - 1] >> 16) & 0xff] << 8);
        rkeys[i + 1] = rkeys[i] ^ rkeys[i - 3];
        rkeys[i + 2] = rkeys[i + 1] ^ rkeys[i - 2];
        rkeys[i + 3] = rkeys[i + 2] ^ rkeys[i - 1];
    }
    // applying ShiftRows^(-i) to match the fully-fixsliced representation
    for (int i = 4; i < 40; i += 4) {
        t0 = rkeys[i];
        t1 = rkeys[i + 1];
        t2 = rkeys[i + 2];
        switch ((i / 4) % 4) {
        case 1:                                         // Applies ShiftRows^(-1)
            rkeys[i]        &= 0x000000ff;
            rkeys[i]        |= rkeys[i + 3] & 0x0000ff00;
            rkeys[i]        |= rkeys[i + 2] & 0x00ff0000;
            rkeys[i]        |= rkeys[i + 1] & 0xff000000;
            rkeys[i + 1]      &= 0x000000ff;
            rkeys[i + 1]      |= t0 & 0x0000ff00;
            rkeys[i + 1]      |= rkeys[i + 3] & 0x00ff0000;
            rkeys[i + 1]      |= rkeys[i + 2] & 0xff000000;
            rkeys[i + 2]      &= 0x000000ff;
            rkeys[i + 2]      |= t1 & 0x0000ff00;
            rkeys[i + 2]      |= t0 & 0x00ff0000;
            rkeys[i + 2]      |= rkeys[i + 3] & 0xff000000;
            rkeys[i + 3]      &= 0x000000ff;
            rkeys[i + 3]      |= t2 & 0x0000ff00;
            rkeys[i + 3]      |= t1 & 0x00ff0000;
            rkeys[i + 3]      |= t0 & 0xff000000;
            break;
        case 2:                                         // Applies ShiftRows^(-2)
            SWAPMOVE(rkeys[i + 2], rkeys[i], 0xff00ff00, 0);
            SWAPMOVE(rkeys[i + 3], rkeys[i + 1], 0xff00ff00, 0);
            break;
        case 3:                                         // Applies ShiftRows^(-3)
            rkeys[i]        &= 0x000000ff;
            rkeys[i]        |= rkeys[i + 1] & 0x0000ff00;
            rkeys[i]        |= rkeys[i + 2] & 0x00ff0000;
            rkeys[i]        |= rkeys[i + 3] & 0xff000000;
            rkeys[i + 1]      &= 0x000000ff;
            rkeys[i + 1]      |= rkeys[i + 3] & 0x00ff0000;
            rkeys[i + 1]      |= rkeys[i + 2] & 0x0000ff00;
            rkeys[i + 1]      |= t0 & 0xff000000;
            rkeys[i + 2]      &= 0x000000ff;
            rkeys[i + 2]      |= rkeys[i + 3] & 0x0000ff00;
            rkeys[i + 2]      |= t0 & 0x00ff0000;
            rkeys[i + 2]      |= t1 & 0xff000000;
            rkeys[i + 3]      &= 0x000000ff;
            rkeys[i + 3]      |= t0 & 0x0000ff00;
            rkeys[i + 3]      |= t1 & 0x00ff0000;
            rkeys[i + 3]      |= t2 & 0xff000000;
            break;
        }
    }
    // packing all round keys to match the fully-fixsliced representation
    packing(rkeys_ffs, (unsigned char *)rkeys, (unsigned char *)rkeys);
    for (int i = 1; i < 11; i++) {
        packing(rkeys_ffs + i * 8, (unsigned char *)(rkeys + i * 4), (unsigned char *)(rkeys + i * 4));
        rkeys_ffs[i * 8 + 1] ^= 0xffffffff;     // NOT to speed up SBox calculations
        rkeys_ffs[i * 8 + 2] ^= 0xffffffff;     // NOT to speed up SBox calculations
        rkeys_ffs[i * 8 + 6] ^= 0xffffffff;     // NOT to speed up SBox calculations
        rkeys_ffs[i * 8 + 7] ^= 0xffffffff;     // NOT to speed up SBox calculations
    }
}




void aes128_4r_keyschedule_ffs_lut(uint32_t *rkeys_ffs, const unsigned char *key) {
    uint32_t t0, t1, t2, tmp;
    uint32_t rkeys[20];
    // key schedule in the classical representation
    rkeys[0] = LE_LOAD_32(key);
    rkeys[1] = LE_LOAD_32(key + 4);
    rkeys[2] = LE_LOAD_32(key + 8);
    rkeys[3] = LE_LOAD_32(key + 12);
    for (int i = 4; i < 20; i += 4) {
        rkeys[i] = rkeys[i - 4] ^ rcon[i / 4];
        rkeys[i] ^= (sbox_lut[rkeys[i - 1] & 0xff] << 24);
        rkeys[i] ^= sbox_lut[(rkeys[i - 1] >> 8) & 0xff];
        rkeys[i] ^= (sbox_lut[rkeys[i - 1] >> 24] << 16);
        rkeys[i] ^= (sbox_lut[(rkeys[i - 1] >> 16) & 0xff] << 8);
        rkeys[i + 1] = rkeys[i] ^ rkeys[i - 3];
        rkeys[i + 2] = rkeys[i + 1] ^ rkeys[i - 2];
        rkeys[i + 3] = rkeys[i + 2] ^ rkeys[i - 1];
    }
    // applying ShiftRows^(-i) to match the fully-fixsliced representation
    for (int i = 4; i < 20; i += 4) {
        t0 = rkeys[i];
        t1 = rkeys[i + 1];
        t2 = rkeys[i + 2];
        switch ((i / 4) % 4) {
        case 1:                                         // Applies ShiftRows^(-1)
            rkeys[i]        &= 0x000000ff;
            rkeys[i]        |= rkeys[i + 3] & 0x0000ff00;
            rkeys[i]        |= rkeys[i + 2] & 0x00ff0000;
            rkeys[i]        |= rkeys[i + 1] & 0xff000000;
            rkeys[i + 1]      &= 0x000000ff;
            rkeys[i + 1]      |= t0 & 0x0000ff00;
            rkeys[i + 1]      |= rkeys[i + 3] & 0x00ff0000;
            rkeys[i + 1]      |= rkeys[i + 2] & 0xff000000;
            rkeys[i + 2]      &= 0x000000ff;
            rkeys[i + 2]      |= t1 & 0x0000ff00;
            rkeys[i + 2]      |= t0 & 0x00ff0000;
            rkeys[i + 2]      |= rkeys[i + 3] & 0xff000000;
            rkeys[i + 3]      &= 0x000000ff;
            rkeys[i + 3]      |= t2 & 0x0000ff00;
            rkeys[i + 3]      |= t1 & 0x00ff0000;
            rkeys[i + 3]      |= t0 & 0xff000000;
            break;
        case 2:                                         // Applies ShiftRows^(-2)
            SWAPMOVE(rkeys[i + 2], rkeys[i], 0xff00ff00, 0);
            SWAPMOVE(rkeys[i + 3], rkeys[i + 1], 0xff00ff00, 0);
            break;
        case 3:                                         // Applies ShiftRows^(-3)
            rkeys[i]        &= 0x000000ff;
            rkeys[i]        |= rkeys[i + 1] & 0x0000ff00;
            rkeys[i]        |= rkeys[i + 2] & 0x00ff0000;
            rkeys[i]        |= rkeys[i + 3] & 0xff000000;
            rkeys[i + 1]      &= 0x000000ff;
            rkeys[i + 1]      |= rkeys[i + 3] & 0x00ff0000;
            rkeys[i + 1]      |= rkeys[i + 2] & 0x0000ff00;
            rkeys[i + 1]      |= t0 & 0xff000000;
            rkeys[i + 2]      &= 0x000000ff;
            rkeys[i + 2]      |= rkeys[i + 3] & 0x0000ff00;
            rkeys[i + 2]      |= t0 & 0x00ff0000;
            rkeys[i + 2]      |= t1 & 0xff000000;
            rkeys[i + 3]      &= 0x000000ff;
            rkeys[i + 3]      |= t0 & 0x0000ff00;
            rkeys[i + 3]      |= t1 & 0x00ff0000;
            rkeys[i + 3]      |= t2 & 0xff000000;
            break;
        }
    }
    // packing all round keys to match the fully-fixsliced representation
    packing(rkeys_ffs, (unsigned char *)rkeys, (unsigned char *)rkeys);
    for (int i = 1; i < 5; i++) {
        packing(rkeys_ffs + i * 8, (unsigned char *)(rkeys + i * 4), (unsigned char *)(rkeys + i * 4));
        rkeys_ffs[i * 8 + 1] ^= 0xffffffff;     // NOT to speed up SBox calculations
        rkeys_ffs[i * 8 + 2] ^= 0xffffffff;     // NOT to speed up SBox calculations
        rkeys_ffs[i * 8 + 6] ^= 0xffffffff;     // NOT to speed up SBox calculations
        rkeys_ffs[i * 8 + 7] ^= 0xffffffff;     // NOT to speed up SBox calculations
    }
}



/******************************************************************************
* C language implementations of the AES-128 and AES-256 key schedules to match
* the fixsliced representation. Note that those implementations are fully
* bitsliced and do not rely on any Look-Up Table (LUT).
*
* See the paper at https://eprint.iacr.org/2020/1123.pdf for more details.
*
* @author   Alexandre Adomnicai, Nanyang Technological University, Singapore
*           alexandre.adomnicai@ntu.edu.sg
*
* @date     October 2020
******************************************************************************/



/******************************************************************************
* Bitsliced implementation of the AES Sbox based on Boyar, Peralta and Calik.
* See http://www.cs.yale.edu/homes/peralta/CircuitStuff/SLP_AES_113.txt
* Note that the 4 NOT (^= 0xffffffff) are moved to the key schedule.
******************************************************************************/
static void sbox(uint32_t *state) {
    uint32_t t0, t1, t2, t3, t4, t5,
             t6, t7, t8, t9, t10, t11, t12,
             t13, t14, t15, t16, t17;
    t0          = state[3] ^ state[5];
    t1          = state[0] ^ state[6];
    t2          = t1 ^ t0;
    t3          = state[4] ^ t2;
    t4          = t3 ^ state[5];
    t5          = t2 & t4;
    t6          = t4 ^ state[7];
    t7          = t3 ^ state[1];
    t8          = state[0] ^ state[3];
    t9          = t7 ^ t8;
    t10         = t8 & t9;
    t11         = state[7] ^ t9;
    t12         = state[0] ^ state[5];
    t13         = state[1] ^ state[2];
    t14         = t4 ^ t13;
    t15         = t14 ^ t9;
    t16         = t0 & t15;
    t17         = t16 ^ t10;
    state[1]    = t14 ^ t12;
    state[2]    = t12 & t14;
    state[2]    ^= t10;
    state[4]    = t13 ^ t9;
    state[5]    = t1 ^ state[4];
    t3          = t1 & state[4];
    t10         = state[0] ^ state[4];
    t13         ^= state[7];
    state[3]    ^= t13;
    t16         = state[3] & state[7];
    t16         ^= t5;
    t16         ^= state[2];
    state[1]    ^= t16;
    state[0]    ^= t13;
    t16         = state[0] & t11;
    t16         ^= t3;
    state[2]    ^= t16;
    state[2]    ^= t10;
    state[6]    ^= t13;
    t10         = state[6] & t13;
    t3          ^= t10;
    t3          ^= t17;
    state[5]    ^= t3;
    t3          = state[6] ^ t12;
    t10         = t3 & t6;
    t5          ^= t10;
    t5          ^= t7;
    t5          ^= t17;
    t7          = t5 & state[5];
    t10         = state[2] ^ t7;
    t7          ^= state[1];
    t5          ^= state[1];
    t16         = t5 & t10;
    state[1]    ^= t16;
    t17         = state[1] & state[0];
    t11         = state[1] & t11;
    t16         = state[5] ^ state[2];
    t7          &= t16;
    t7          ^= state[2];
    t16         = t10 ^ t7;
    state[2]    &= t16;
    t10         ^= state[2];
    t10         &= state[1];
    t5          ^= t10;
    t10         = state[1] ^ t5;
    state[4]    &= t10;
    t11         ^= state[4];
    t1          &= t10;
    state[6]    &= t5;
    t10         = t5 & t13;
    state[4]    ^= t10;
    state[5]    ^= t7;
    state[2]    ^= state[5];
    state[5]    = t5 ^ state[2];
    t5          = state[5] & t14;
    t10         = state[5] & t12;
    t12         = t7 ^ state[2];
    t4          &= t12;
    t2          &= t12;
    t3          &= state[2];
    state[2]    &= t6;
    state[2]    ^= t4;
    t13         = state[4] ^ state[2];
    state[3]    &= t7;
    state[1]    ^= t7;
    state[5]    ^= state[1];
    t6          = state[5] & t15;
    state[4]    ^= t6;
    t0          &= state[5];
    state[5]    = state[1] & t9;
    state[5]    ^= state[4];
    state[1]    &= t8;
    t6          = state[1] ^ state[5];
    t0          ^= state[1];
    state[1]    = t3 ^ t0;
    t15         = state[1] ^ state[3];
    t2          ^= state[1];
    state[0]    = t2 ^ state[5];
    state[3]    = t2 ^ t13;
    state[1]    = state[3] ^ state[5];
    //state[1]  ^= 0xffffffff;
    t0          ^= state[6];
    state[5]    = t7 & state[7];
    t14         = t4 ^ state[5];
    state[6]    = t1 ^ t14;
    state[6]    ^= t5;
    state[6]    ^= state[4];
    state[2]    = t17 ^ state[6];
    state[5]    = t15 ^ state[2];
    state[2]    ^= t6;
    state[2]    ^= t10;
    //state[2]  ^= 0xffffffff;
    t14         ^= t11;
    t0          ^= t14;
    state[6]    ^= t0;
    //state[6]  ^= 0xffffffff;
    state[7]    = t1 ^ t0;
    //state[7]  ^= 0xffffffff;
    state[4]    = t14 ^ state[3];
}






///////////////////////////////////////////////////////////////////////////////////////






#define BYTE_ROR_6(x)                                                                           \
    ((((x) >> 6) & 0x03030303)      | (((x) & 0x3f3f3f3f) << 2))

#define BYTE_ROR_4(x)                                                                           \
    ((((x) >> 4) & 0x0f0f0f0f)      | (((x) & 0x0f0f0f0f) << 4))

#define BYTE_ROR_2(x)                                                                           \
    ((((x) >> 2) & 0x3f3f3f3f)      | (((x) & 0x03030303) << 6))



#define LE_STORE_32(x, y)                                                                       \
    (x)[0] = (y) & 0xff;                                                                    \
    (x)[1] = ((y) >> 8) & 0xff;                                                     \
    (x)[2] = ((y) >> 16) & 0xff;                                                    \
    (x)[3] = (y) >> 24;




/******************************************************************************
* Unpacks the 256-bit internal state in two 128-bit blocs out0, out1.
******************************************************************************/
static void unpacking(unsigned char *out0, unsigned char *out1, uint32_t *in) {
    uint32_t tmp;
    SWAPMOVE(in[4], in[0], 0x0f0f0f0f, 4);
    SWAPMOVE(in[5], in[1], 0x0f0f0f0f, 4);
    SWAPMOVE(in[6], in[2], 0x0f0f0f0f, 4);
    SWAPMOVE(in[7], in[3], 0x0f0f0f0f, 4);
    SWAPMOVE(in[2], in[0], 0x33333333, 2);
    SWAPMOVE(in[3], in[1], 0x33333333, 2);
    SWAPMOVE(in[6], in[4], 0x33333333, 2);
    SWAPMOVE(in[7], in[5], 0x33333333, 2);
    SWAPMOVE(in[1], in[0], 0x55555555, 1);
    SWAPMOVE(in[3], in[2], 0x55555555, 1);
    SWAPMOVE(in[5], in[4], 0x55555555, 1);
    SWAPMOVE(in[7], in[6], 0x55555555, 1);
    LE_STORE_32(out0, in[0]);
    LE_STORE_32(out0 + 4, in[2]);
    LE_STORE_32(out0 + 8, in[4]);
    LE_STORE_32(out0 + 12, in[6]);
    LE_STORE_32(out1, in[1]);
    LE_STORE_32(out1 + 4, in[3]);
    LE_STORE_32(out1 + 8, in[5]);
    LE_STORE_32(out1 + 12, in[7]);
}

/******************************************************************************
* XOR the round key to the internal state. The round keys are expected to be
* pre-computed and to be packed in the fixsliced representation.
******************************************************************************/
static void ark(uint32_t *state, const uint32_t *rkey) {
    for (int i = 0; i < 8; i++) {
        state[i] ^= rkey[i];
    }
}


/******************************************************************************
* Applies the ShiftRows transformation twice (i.e. SR^2) on the internal state.
******************************************************************************/
static void double_shiftrows(uint32_t *state) {
    uint32_t tmp;
    for (int i = 0; i < 8; i++) {
        SWAPMOVE(state[i], state[i], 0x0f000f00, 4);
    }
}

/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations, it is used for rounds i s.t. (i%4) == 0.
* For semi-fixsliced implementations, it is used for rounds i s.t. (i%2) == 0.
******************************************************************************/
static void mixcolumns_0(uint32_t *state) {
    uint32_t t0, t1, t2, t3, t4;
    t3 = ROR(BYTE_ROR_6(state[0]), 8);
    t0 = state[0] ^ t3;
    t1 = ROR(BYTE_ROR_6(state[7]), 8);
    t2 = state[7] ^ t1;
    state[7] = ROR(BYTE_ROR_4(t2), 16) ^ t1 ^ t0;
    t1 = ROR(BYTE_ROR_6(state[6]), 8);
    t4 = t1 ^ state[6];
    state[6] = t2 ^ t0 ^ t1 ^ ROR(BYTE_ROR_4(t4), 16);
    t1 = ROR(BYTE_ROR_6(state[5]), 8);
    t2 = t1 ^ state[5];
    state[5] = t4 ^ t1 ^ ROR(BYTE_ROR_4(t2), 16);
    t1 = ROR(BYTE_ROR_6(state[4]), 8);
    t4 = t1 ^ state[4];
    state[4] = t2 ^ t0 ^ t1 ^ ROR(BYTE_ROR_4(t4), 16);
    t1 = ROR(BYTE_ROR_6(state[3]), 8);
    t2 = t1 ^ state[3];
    state[3] = t4 ^ t0 ^ t1 ^ ROR(BYTE_ROR_4(t2), 16);
    t1 = ROR(BYTE_ROR_6(state[2]), 8);
    t4 = t1 ^ state[2];
    state[2] = t2 ^ t1 ^ ROR(BYTE_ROR_4(t4), 16);
    t1 = ROR(BYTE_ROR_6(state[1]), 8);
    t2 = t1 ^ state[1];
    state[1] = t4 ^ t1 ^ ROR(BYTE_ROR_4(t2), 16);
    state[0] = t2 ^ t3 ^ ROR(BYTE_ROR_4(t0), 16);
}

/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations only, for round i s.t. (i%4) == 1.
******************************************************************************/
static void mixcolumns_1(uint32_t *state) {
    uint32_t t0, t1, t2;
    t0 = state[0] ^ ROR(BYTE_ROR_4(state[0]), 8);
    t1 = state[7] ^ ROR(BYTE_ROR_4(state[7]), 8);
    t2 = state[6];
    state[6] = t1 ^ t0;
    state[7] ^= state[6] ^ ROR(t1, 16);
    t1 =  ROR(BYTE_ROR_4(t2), 8);
    state[6] ^= t1;
    t1 ^= t2;
    state[6] ^= ROR(t1, 16);
    t2 = state[5];
    state[5] = t1;
    t1 =  ROR(BYTE_ROR_4(t2), 8);
    state[5] ^= t1;
    t1 ^= t2;
    state[5] ^= ROR(t1, 16);
    t2 = state[4];
    state[4] = t1 ^ t0;
    t1 =  ROR(BYTE_ROR_4(t2), 8);
    state[4] ^= t1;
    t1 ^= t2;
    state[4] ^= ROR(t1, 16);
    t2 = state[3];
    state[3] = t1 ^ t0;
    t1 =  ROR(BYTE_ROR_4(t2), 8);
    state[3] ^= t1;
    t1 ^= t2;
    state[3] ^= ROR(t1, 16);
    t2 = state[2];
    state[2] = t1;
    t1 = ROR(BYTE_ROR_4(t2), 8);
    state[2] ^= t1;
    t1 ^= t2;
    state[2] ^= ROR(t1, 16);
    t2 = state[1];
    state[1] = t1;
    t1 = ROR(BYTE_ROR_4(t2), 8);
    state[1] ^= t1;
    t1 ^= t2;
    state[1] ^= ROR(t1, 16);
    t2 = state[0];
    state[0] = t1;
    t1 = ROR(BYTE_ROR_4(t2), 8);
    state[0] ^= t1;
    t1 ^= t2;
    state[0] ^= ROR(t1, 16);
}

/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations only, for rounds i s.t. (i%4) == 2.
******************************************************************************/
static void mixcolumns_2(uint32_t *state) {
    uint32_t t0, t1, t2, t3, t4;
    t3 = ROR(BYTE_ROR_2(state[0]), 8);
    t0 = state[0] ^ t3;
    t1 = ROR(BYTE_ROR_2(state[7]), 8);
    t2 = state[7] ^ t1;
    state[7] = ROR(BYTE_ROR_4(t2), 16) ^ t1 ^ t0;
    t1 = ROR(BYTE_ROR_2(state[6]), 8);
    t4 = t1 ^ state[6];
    state[6] = t2 ^ t0 ^ t1 ^ ROR(BYTE_ROR_4(t4), 16);
    t1 = ROR(BYTE_ROR_2(state[5]), 8);
    t2 = t1 ^ state[5];
    state[5] = t4 ^ t1 ^ ROR(BYTE_ROR_4(t2), 16);
    t1 = ROR(BYTE_ROR_2(state[4]), 8);
    t4 = t1 ^ state[4];
    state[4] = t2 ^ t0 ^ t1 ^ ROR(BYTE_ROR_4(t4), 16);
    t1 = ROR(BYTE_ROR_2(state[3]), 8);
    t2 = t1 ^ state[3];
    state[3] = t4 ^ t0 ^ t1 ^ ROR(BYTE_ROR_4(t2), 16);
    t1 = ROR(BYTE_ROR_2(state[2]), 8);
    t4 = t1 ^ state[2];
    state[2] = t2 ^ t1 ^ ROR(BYTE_ROR_4(t4), 16);
    t1 = ROR(BYTE_ROR_2(state[1]), 8);
    t2 = t1 ^ state[1];
    state[1] = t4 ^ t1 ^ ROR(BYTE_ROR_4(t2), 16);
    state[0] = t2 ^ t3 ^ ROR(BYTE_ROR_4(t0), 16);
}

/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations, it is used for rounds i s.t. (i%4) == 3.
* For semi-fixsliced implementations, it is used for rounds i s.t. (i%2) == 1.
* Based on Käsper-Schwabe, similar to https://github.com/Ko-/aes-armcortexm.
******************************************************************************/
static void mixcolumns_3(uint32_t *state) {
    uint32_t t0, t1, t2;
    t0 = state[7] ^ ROR(state[7], 8);
    t2 = state[0] ^ ROR(state[0], 8);
    state[7] = t2 ^ ROR(state[7], 8) ^ ROR(t0, 16);
    t1 = state[6] ^ ROR(state[6], 8);
    state[6] = t0 ^ t2 ^ ROR(state[6], 8) ^ ROR(t1, 16);
    t0 = state[5] ^ ROR(state[5], 8);
    state[5] = t1 ^ ROR(state[5], 8) ^ ROR(t0, 16);
    t1 = state[4] ^ ROR(state[4], 8);
    state[4] = t0 ^ t2 ^ ROR(state[4], 8) ^ ROR(t1, 16);
    t0 = state[3] ^ ROR(state[3], 8);
    state[3] = t1 ^ t2 ^ ROR(state[3], 8) ^ ROR(t0, 16);
    t1 = state[2] ^ ROR(state[2], 8);
    state[2] = t0 ^ ROR(state[2], 8) ^ ROR(t1, 16);
    t0 = state[1] ^ ROR(state[1], 8);
    state[1] = t1 ^ ROR(state[1], 8) ^ ROR(t0, 16);
    state[0] = t0 ^ ROR(state[0], 8) ^ ROR(t2, 16);
}


/******************************************************************************
* Fully-fixsliced AES-128 encryption (the ShiftRows is completely omitted).
* Two 128-bit blocks ptext0, ptext1 are encrypted into ctext0, ctext1 without
* any operating mode. The round keys are assumed to be pre-computed.
* Note that it can be included in serial operating modes since ptext0, ptext1
* can refer to the same block. Moreover ctext parameters can be the same as
* ptext parameters.
******************************************************************************/
void aes128_encrypt_ffs(unsigned char *ctext0, unsigned char *ctext1,
                        const unsigned char *ptext0, const unsigned char *ptext1,
                        const uint32_t *rkeys_ffs) {
    uint32_t state[8];                                      // 256-bit internal state
    packing(state, ptext0, ptext1);         // packs into bitsliced representation
    ark(state, rkeys_ffs);                          // key whitening
    sbox(state);                                            // 1st round
    mixcolumns_0(state);                            // 1st round
    ark(state, rkeys_ffs + 8);                      // 1st round
    sbox(state);                                            // 2nd round
    mixcolumns_1(state);                            // 2nd round
    ark(state, rkeys_ffs + 16);             // 2nd round
    sbox(state);                                            // 3rd round
    mixcolumns_2(state);                            // 3rd round
    ark(state, rkeys_ffs + 24);             // 3rd round
    sbox(state);                                            // 4th round
    mixcolumns_3(state);                            // 4th round
    ark(state, rkeys_ffs + 32);             // 4th round
    sbox(state);                                            // 5th round
    mixcolumns_0(state);                            // 5th round
    ark(state, rkeys_ffs + 40);             // 5th round
    sbox(state);                                            // 6th round
    mixcolumns_1(state);                            // 6th round
    ark(state, rkeys_ffs + 48);             // 6th round
    sbox(state);                                            // 7th round
    mixcolumns_2(state);                            // 7th round
    ark(state, rkeys_ffs + 56);             // 7th round
    sbox(state);                                            // 8th round
    mixcolumns_3(state);                            // 8th round
    ark(state, rkeys_ffs + 64);             // 8th round
    sbox(state);                                            // 9th round
    mixcolumns_0(state);                            // 9th round
    ark(state, rkeys_ffs + 72);             // 9th round
    sbox(state);                                            // 10th round
    double_shiftrows(state);                        // 10th round (resynchronization)
    ark(state, rkeys_ffs + 80);             // 10th round
    unpacking(ctext0, ctext1, state);       // unpacks the state to the output
}


void aes128_4r_encrypt_ffs(unsigned char *ctext0, unsigned char *ctext1,
                           const unsigned char *ptext0, const unsigned char *ptext1,
                           const uint32_t *rkeys_ffs) {
    uint32_t state[8];                                      // 256-bit internal state
    packing(state, ptext0, ptext1);         // packs into bitsliced representation
    ark(state, rkeys_ffs);                          // key whitening
    sbox(state);                                            // 1st round
    mixcolumns_0(state);                            // 1st round
    ark(state, rkeys_ffs + 8);                      // 1st round
    sbox(state);                                            // 2nd round
    mixcolumns_1(state);                            // 2nd round
    ark(state, rkeys_ffs + 16);             // 2nd round
    sbox(state);                                            // 3rd round
    mixcolumns_2(state);                            // 3rd round
    ark(state, rkeys_ffs + 24);             // 3rd round
    sbox(state);                                            // 4th round
    //mixcolumns_3(state);                            // 4th round
    ark(state, rkeys_ffs + 32);             // 4th round
    unpacking(ctext0, ctext1, state);       // unpacks the state to the output
}


