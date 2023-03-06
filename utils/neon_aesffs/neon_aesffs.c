/******************************************************************************
* Fixsliced implementations of AES-128  and AES-256 (encryption-only) in C.
* Fully-fixsliced implementation should run faster than the semi-fixsliced
* version at the cost of a bigger code size.
*
* See the paper at https://eprint.iacr.org/2020/1123.pdf for more details.
*
* @author   Alexandre Adomnicai, Nanyang Technological University, Singapore
*           alexandre.adomnicai@ntu.edu.sg
*
* @date     October 2020
******************************************************************************/


#include "arm_neon.h"
#include "neon_aesffs.h"


#define LE_LOAD_32(x)                                       \
    ((((uint32_t)((x)[3])) << 24) |                         \
     (((uint32_t)((x)[2])) << 16) |                         \
     (((uint32_t)((x)[1])) << 8) |                          \
     ((uint32_t)((x)[0])))


#define LE_STORE_32(x, y)                                   \
    (x)[0] = (y) & 0xff;                                    \
    (x)[1] = ((y) >> 8) & 0xff;                             \
    (x)[2] = ((y) >> 16) & 0xff;                            \
    (x)[3] = (y) >> 24;




#define _SWAPMOVE_1(a,b,ignore_mask,ignore_n) do { \
        tmp = (b^vshrq_n_u32(a,1))&mask55;               \
        b = b^tmp;                                       \
        a = a^vshlq_n_u32(tmp,1);                        \
    } while(0)

#define _SWAPMOVE_2(a,b,ignore_mask,ignore_n) do { \
        tmp = (b^vshrq_n_u32(a,2))&mask33;               \
        b = b^tmp;                                       \
        a = a^vshlq_n_u32(tmp,2);                        \
    } while(0)

#define _SWAPMOVE_4(a,b,ignore_mask,ignore_n) do { \
        tmp = vreinterpretq_u32_u8(vsriq_n_u8( vreinterpretq_u8_u32(b),  vreinterpretq_u8_u32(a), 4 ));   \
        a = vreinterpretq_u32_u8(vsliq_n_u8( vreinterpretq_u8_u32(a),  vreinterpretq_u8_u32(b), 4 ));   \
        b = tmp;                        \
    } while(0)





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
static void _packing(uint32x4_t *out, const uint8_t *iv, const uint32_t aesctr_ctr) {
    uint32x4_t tmp;
    uint32x4_t mask55 = vdupq_n_u32(0x55555555);
    uint32x4_t mask33 = vdupq_n_u32(0x33333333);
    uint32_t ctrs[8];
    for (unsigned i = 0; i < 8; i++) {
        ctrs[i] = aesctr_ctr + i;
    }
    uint32x4x2_t nctr = vld2q_u32( ctrs );
    uint8x16_t nonce = vld1q_u8( iv );

    out[0] = vdupq_laneq_u32(nonce, 0);
    out[1] = out[0];
    out[2] = vdupq_laneq_u32(nonce, 1);
    out[3] = out[2];
    out[4] = vdupq_laneq_u32(nonce, 2);
    out[5] = out[4];
    out[6] = vrev32q_u8( nctr.val[0] );
    out[7] = vrev32q_u8( nctr.val[1] );
    _SWAPMOVE_1(out[1], out[0], 0x55555555, 1);
    _SWAPMOVE_1(out[3], out[2], 0x55555555, 1);
    _SWAPMOVE_1(out[5], out[4], 0x55555555, 1);
    _SWAPMOVE_1(out[7], out[6], 0x55555555, 1);
    _SWAPMOVE_2(out[2], out[0], 0x33333333, 2);
    _SWAPMOVE_2(out[3], out[1], 0x33333333, 2);
    _SWAPMOVE_2(out[6], out[4], 0x33333333, 2);
    _SWAPMOVE_2(out[7], out[5], 0x33333333, 2);
    _SWAPMOVE_4(out[4], out[0], 0x0f0f0f0f, 4);
    _SWAPMOVE_4(out[5], out[1], 0x0f0f0f0f, 4);
    _SWAPMOVE_4(out[6], out[2], 0x0f0f0f0f, 4);
    _SWAPMOVE_4(out[7], out[3], 0x0f0f0f0f, 4);
}

static void unpacking(unsigned char *out, uint32x4_t *in) {
    uint32x4_t tmp;
    uint32x4_t mask55 = vdupq_n_u32(0x55555555);
    uint32x4_t mask33 = vdupq_n_u32(0x33333333);

    _SWAPMOVE_4(in[4], in[0], 0x0f0f0f0f, 4);
    _SWAPMOVE_4(in[5], in[1], 0x0f0f0f0f, 4);
    _SWAPMOVE_4(in[6], in[2], 0x0f0f0f0f, 4);
    _SWAPMOVE_4(in[7], in[3], 0x0f0f0f0f, 4);
    _SWAPMOVE_2(in[2], in[0], 0x33333333, 2);
    _SWAPMOVE_2(in[3], in[1], 0x33333333, 2);
    _SWAPMOVE_2(in[6], in[4], 0x33333333, 2);
    _SWAPMOVE_2(in[7], in[5], 0x33333333, 2);
    _SWAPMOVE_1(in[1], in[0], 0x55555555, 1);
    _SWAPMOVE_1(in[3], in[2], 0x55555555, 1);
    _SWAPMOVE_1(in[5], in[4], 0x55555555, 1);
    _SWAPMOVE_1(in[7], in[6], 0x55555555, 1);

    tmp = vtrn1q_u32( in[0], in[2] );
    in[2] = vtrn2q_u32( in[0], in[2] );
    in[0] = tmp;
    tmp = vtrn1q_u32( in[1], in[3] );
    in[3] = vtrn2q_u32( in[1], in[3] );
    in[1] = tmp;
    tmp = vtrn1q_u32( in[4], in[6] );
    in[6] = vtrn2q_u32( in[4], in[6] );
    in[4] = tmp;
    tmp = vtrn1q_u32( in[5], in[7] );
    in[7] = vtrn2q_u32( in[5], in[7] );
    in[5] = tmp;

    tmp = vtrn1q_u64( in[0], in[4] );
    in[4] = vtrn2q_u64( in[0], in[4] );
    in[0] = tmp;
    tmp = vtrn1q_u64( in[1], in[5] );
    in[5] = vtrn2q_u64( in[1], in[5] );
    in[1] = tmp;
    tmp = vtrn1q_u64( in[2], in[6] );
    in[6] = vtrn2q_u64( in[2], in[6] );
    in[2] = tmp;
    tmp = vtrn1q_u64( in[3], in[7] );
    in[7] = vtrn2q_u64( in[3], in[7] );
    in[3] = tmp;

    for (int i = 0; i < 8; i++) {
        vst1q_u8( out + i * 16, in[i] );
    }
}




/******************************************************************************
* XOR the round key to the internal state. The round keys are expected to be
* pre-computed and to be packed in the fixsliced representation.
******************************************************************************/
static inline void ark(uint32x4_t *state, const uint32_t *rkey) {
    uint32x4_t k0 = vld1q_u32(rkey);
    uint32x4_t k1 = vld1q_u32(rkey + 4);

    state[0] ^= vdupq_laneq_u32(k0, 0);
    state[1] ^= vdupq_laneq_u32(k0, 1);
    state[2] ^= vdupq_laneq_u32(k0, 2);
    state[3] ^= vdupq_laneq_u32(k0, 3);

    state[4] ^= vdupq_laneq_u32(k1, 0);
    state[5] ^= vdupq_laneq_u32(k1, 1);
    state[6] ^= vdupq_laneq_u32(k1, 2);
    state[7] ^= vdupq_laneq_u32(k1, 3);
}

/******************************************************************************
* Bitsliced implementation of the AES Sbox based on Boyar, Peralta and Calik.
* See http://www.cs.yale.edu/homes/peralta/CircuitStuff/SLP_AES_113.txt
* Note that the 4 NOT (^= 0xffffffff) are moved to the key schedule.
******************************************************************************/
static inline void _sbox(uint32x4_t *state) {
    uint32x4_t t0, t1, t2, t3, t4, t5,
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


/******************************************************************************
* Applies the ShiftRows transformation twice (i.e. SR^2) on the internal state.
******************************************************************************/

static inline void double_shiftrows(uint32x4_t *state) {
    uint32x4_t mask = vdupq_n_u32(0x0f000f00);
    for (int i = 0; i < 8; i++) {
        uint32x4_t temp = (vshrq_n_u32(state[i], 4) ^ state[i]) & mask;
        state[i] ^= temp;
        state[i] ^= vshlq_n_u32(temp, 4);
    }
}


static inline
uint32x4_t _BYTE_ROR_6(uint8x16_t a) {
    return vreinterpretq_u32_u8(vsliq_n_u8( vshrq_n_u8(vreinterpretq_u8_u32(a), 6),  vreinterpretq_u8_u32(a), 2 ));
}

static inline
uint32x4_t _BYTE_ROR_4(uint8x16_t a) {
    return vreinterpretq_u32_u8(vsliq_n_u8( vshrq_n_u8(vreinterpretq_u8_u32(a), 4),  vreinterpretq_u8_u32(a), 4 ));
}

static inline
uint32x4_t _BYTE_ROR_2(uint8x16_t a) {
    return vreinterpretq_u32_u8(vsliq_n_u8( vshrq_n_u8(vreinterpretq_u8_u32(a), 2),  vreinterpretq_u8_u32(a), 6 ));
}

static inline
uint32x4_t _ROR32_8(uint32x4_t a, int b) {
    (void)b;
    return vsliq_n_u32( vshrq_n_u32(a, 8), a, 24);
}

static inline
uint32x4_t _ROR32_16(uint32x4_t a, int b) {
    (void )b;
    return vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(a)));
}


/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations, it is used for rounds i s.t. (i%4) == 0.
* For semi-fixsliced implementations, it is used for rounds i s.t. (i%2) == 0.
******************************************************************************/
static void mixcolumns_0(uint32x4_t *state) {
    uint32x4_t t0, t1, t2, t3, t4;
    t3 = _ROR32_8(_BYTE_ROR_6(state[0]), 8);
    t0 = state[0] ^ t3;
    t1 = _ROR32_8(_BYTE_ROR_6(state[7]), 8);
    t2 = state[7] ^ t1;
    state[7] = _ROR32_16(_BYTE_ROR_4(t2), 16) ^ t1 ^ t0;
    t1 = _ROR32_8(_BYTE_ROR_6(state[6]), 8);
    t4 = t1 ^ state[6];
    state[6] = t2 ^ t0 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t4), 16);
    t1 = _ROR32_8(_BYTE_ROR_6(state[5]), 8);
    t2 = t1 ^ state[5];
    state[5] = t4 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t2), 16);
    t1 = _ROR32_8(_BYTE_ROR_6(state[4]), 8);
    t4 = t1 ^ state[4];
    state[4] = t2 ^ t0 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t4), 16);
    t1 = _ROR32_8(_BYTE_ROR_6(state[3]), 8);
    t2 = t1 ^ state[3];
    state[3] = t4 ^ t0 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t2), 16);
    t1 = _ROR32_8(_BYTE_ROR_6(state[2]), 8);
    t4 = t1 ^ state[2];
    state[2] = t2 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t4), 16);
    t1 = _ROR32_8(_BYTE_ROR_6(state[1]), 8);
    t2 = t1 ^ state[1];
    state[1] = t4 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t2), 16);
    state[0] = t2 ^ t3 ^ _ROR32_16(_BYTE_ROR_4(t0), 16);
}
/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations only, for round i s.t. (i%4) == 1.
******************************************************************************/
static void mixcolumns_1(uint32x4_t *state) {
    uint32x4_t t0, t1, t2;
    t0 = state[0] ^ _ROR32_8(_BYTE_ROR_4(state[0]), 8);
    t1 = state[7] ^ _ROR32_8(_BYTE_ROR_4(state[7]), 8);
    t2 = state[6];
    state[6] = t1 ^ t0;
    state[7] ^= state[6] ^ _ROR32_16(t1, 16);
    t1 =  _ROR32_8(_BYTE_ROR_4(t2), 8);
    state[6] ^= t1;
    t1 ^= t2;
    state[6] ^= _ROR32_16(t1, 16);
    t2 = state[5];
    state[5] = t1;
    t1 =  _ROR32_8(_BYTE_ROR_4(t2), 8);
    state[5] ^= t1;
    t1 ^= t2;
    state[5] ^= _ROR32_16(t1, 16);
    t2 = state[4];
    state[4] = t1 ^ t0;
    t1 =  _ROR32_8(_BYTE_ROR_4(t2), 8);
    state[4] ^= t1;
    t1 ^= t2;
    state[4] ^= _ROR32_16(t1, 16);
    t2 = state[3];
    state[3] = t1 ^ t0;
    t1 =  _ROR32_8(_BYTE_ROR_4(t2), 8);
    state[3] ^= t1;
    t1 ^= t2;
    state[3] ^= _ROR32_16(t1, 16);
    t2 = state[2];
    state[2] = t1;
    t1 = _ROR32_8(_BYTE_ROR_4(t2), 8);
    state[2] ^= t1;
    t1 ^= t2;
    state[2] ^= _ROR32_16(t1, 16);
    t2 = state[1];
    state[1] = t1;
    t1 = _ROR32_8(_BYTE_ROR_4(t2), 8);
    state[1] ^= t1;
    t1 ^= t2;
    state[1] ^= _ROR32_16(t1, 16);
    t2 = state[0];
    state[0] = t1;
    t1 = _ROR32_8(_BYTE_ROR_4(t2), 8);
    state[0] ^= t1;
    t1 ^= t2;
    state[0] ^= _ROR32_16(t1, 16);
}
/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations only, for rounds i s.t. (i%4) == 2.
******************************************************************************/
static void mixcolumns_2(uint32x4_t *state) {
    uint32x4_t t0, t1, t2, t3, t4;
    t3 = _ROR32_8(_BYTE_ROR_2(state[0]), 8);
    t0 = state[0] ^ t3;
    t1 = _ROR32_8(_BYTE_ROR_2(state[7]), 8);
    t2 = state[7] ^ t1;
    state[7] = _ROR32_16(_BYTE_ROR_4(t2), 16) ^ t1 ^ t0;
    t1 = _ROR32_8(_BYTE_ROR_2(state[6]), 8);
    t4 = t1 ^ state[6];
    state[6] = t2 ^ t0 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t4), 16);
    t1 = _ROR32_8(_BYTE_ROR_2(state[5]), 8);
    t2 = t1 ^ state[5];
    state[5] = t4 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t2), 16);
    t1 = _ROR32_8(_BYTE_ROR_2(state[4]), 8);
    t4 = t1 ^ state[4];
    state[4] = t2 ^ t0 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t4), 16);
    t1 = _ROR32_8(_BYTE_ROR_2(state[3]), 8);
    t2 = t1 ^ state[3];
    state[3] = t4 ^ t0 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t2), 16);
    t1 = _ROR32_8(_BYTE_ROR_2(state[2]), 8);
    t4 = t1 ^ state[2];
    state[2] = t2 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t4), 16);
    t1 = _ROR32_8(_BYTE_ROR_2(state[1]), 8);
    t2 = t1 ^ state[1];
    state[1] = t4 ^ t1 ^ _ROR32_16(_BYTE_ROR_4(t2), 16);
    state[0] = t2 ^ t3 ^ _ROR32_16(_BYTE_ROR_4(t0), 16);
}
/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations, it is used for rounds i s.t. (i%4) == 3.
* For semi-fixsliced implementations, it is used for rounds i s.t. (i%2) == 1.
* Based on Käsper-Schwabe, similar to https://github.com/Ko-/aes-armcortexm.
******************************************************************************/
static void mixcolumns_3(uint32x4_t *state) {
    uint32x4_t t0, t1, t2;
    t0 = state[7] ^ _ROR32_8(state[7], 8);
    t2 = state[0] ^ _ROR32_8(state[0], 8);
    state[7] = t2 ^ _ROR32_8(state[7], 8) ^ _ROR32_16(t0, 16);
    t1 = state[6] ^ _ROR32_8(state[6], 8);
    state[6] = t0 ^ t2 ^ _ROR32_8(state[6], 8) ^ _ROR32_16(t1, 16);
    t0 = state[5] ^ _ROR32_8(state[5], 8);
    state[5] = t1 ^ _ROR32_8(state[5], 8) ^ _ROR32_16(t0, 16);
    t1 = state[4] ^ _ROR32_8(state[4], 8);
    state[4] = t0 ^ t2 ^ _ROR32_8(state[4], 8) ^ _ROR32_16(t1, 16);
    t0 = state[3] ^ _ROR32_8(state[3], 8);
    state[3] = t1 ^ t2 ^ _ROR32_8(state[3], 8) ^ _ROR32_16(t0, 16);
    t1 = state[2] ^ _ROR32_8(state[2], 8);
    state[2] = t0 ^ _ROR32_8(state[2], 8) ^ _ROR32_16(t1, 16);
    t0 = state[1] ^ _ROR32_8(state[1], 8);
    state[1] = t1 ^ _ROR32_8(state[1], 8) ^ _ROR32_16(t0, 16);
    state[0] = t0 ^ _ROR32_8(state[0], 8) ^ _ROR32_16(t2, 16);
}



/******************************************************************************
* Fully-fixsliced AES-256 encryption (the ShiftRows is completely omitted).
* Two 128-bit blocks ptext0, ptext1 are encrypted into ctext0, ctext1 without
* any operating mode. The round keys are assumed to be pre-computed.
* Note that it can be included in serial operating modes since ptext0, ptext1
* can refer to the same block. Moreover ctext parameters can be the same as
* ptext parameters.
******************************************************************************/

void neon_aes128ctrx8_encrypt_ffs(unsigned char *ctext0, const uint8_t *iv, uint32_t ctr, const uint32_t *rkeys_ffs) {
    uint32x4_t state[8];               // 256-bit internal state
    _packing(state, iv, ctr);       // packs into bitsliced representation

    for (int i = 0; i < 64; i += 32) {  // loop over quadruple rounds
        ark(state, rkeys_ffs + i);
        _sbox(state);
        mixcolumns_0(state);
        ark(state, rkeys_ffs + i + 8);
        _sbox(state);
        mixcolumns_1(state);
        ark(state, rkeys_ffs + i + 16);
        _sbox(state);
        mixcolumns_2(state);
        ark(state, rkeys_ffs + i + 24);
        _sbox(state);
        mixcolumns_3(state);
    }
    ark(state, rkeys_ffs + 64);
    _sbox(state);
    mixcolumns_0(state);
    ark(state, rkeys_ffs + 72);
    _sbox(state);
    double_shiftrows(state);            // resynchronization
    ark(state, rkeys_ffs + 80);

    unpacking(ctext0, state);   // unpacks the state to the output
}


void neon_aes128ctrx8_4r_encrypt_ffs(unsigned char *ctext0, const uint8_t *iv, uint32_t ctr, const uint32_t *rkeys_ffs) {
    uint32x4_t state[8];               // 256-bit internal state
    _packing(state, iv, ctr);       // packs into bitsliced representation

    for (int i = 0; i < 32; i += 32) {  // loop over quadruple rounds
        ark(state, rkeys_ffs + i);
        _sbox(state);
        mixcolumns_0(state);
        ark(state, rkeys_ffs + i + 8);
        _sbox(state);
        mixcolumns_1(state);
        ark(state, rkeys_ffs + i + 16);
        _sbox(state);
        mixcolumns_2(state);
        ark(state, rkeys_ffs + i + 24);
        _sbox(state);
        //mixcolumns_3(state);
    }
    ark(state, rkeys_ffs + 32);
    unpacking(ctext0, state);   // unpacks the state to the output
}

