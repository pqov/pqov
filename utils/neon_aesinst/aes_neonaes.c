

#include "aes_neonaes.h"

#include "arm_neon.h"



void aes256ctrx4_enc_neonaes( uint8_t *ct, const uint8_t *nonce, uint32_t ctr, const uint8_t *rk ) {
    static const uint32_t mask[4] = {0, 1, 2, 3};
    uint32x4_t idx   = vld1q_u32( mask );
    uint32x4_t ctrx4 = vdupq_n_u32( ctr );
    uint32x4_t ctrx4_be = vreinterpretq_u32_u8(vrev32q_u8( vreinterpretq_u8_u32(vaddq_u32(ctrx4, idx)) )); // to big endian numbers

    uint32x4_t iv = vreinterpretq_u32_u8(vld1q_u8(nonce));
    uint8x16_t p0 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 0 ));
    uint8x16_t p1 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 1 ));
    uint8x16_t p2 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 2 ));
    uint8x16_t p3 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 3 ));

    uint8x16_t rki;
    for (int i = 0; i < 13; i++) {
        rki = vld1q_u8(rk);
        rk += 16;
        p0 = vaeseq_u8( p0, rki );
        p1 = vaeseq_u8( p1, rki );
        p2 = vaeseq_u8( p2, rki );
        p3 = vaeseq_u8( p3, rki );
        p0 = vaesmcq_u8( p0 );
        p1 = vaesmcq_u8( p1 );
        p2 = vaesmcq_u8( p2 );
        p3 = vaesmcq_u8( p3 );
    }
    rki = vld1q_u8(rk);
    rk += 16;
    p0 = vaeseq_u8( p0, rki );
    p1 = vaeseq_u8( p1, rki );
    p2 = vaeseq_u8( p2, rki );
    p3 = vaeseq_u8( p3, rki );
    rki = vld1q_u8(rk);
    p0 ^= rki;
    p1 ^= rki;
    p2 ^= rki;
    p3 ^= rki;
    vst1q_u8( ct, p0 );
    vst1q_u8( ct + 16, p1 );
    vst1q_u8( ct + 32, p2 );
    vst1q_u8( ct + 48, p3 );
}

void aes128ctrx4_enc_neonaes( uint8_t *ct, const uint8_t *nonce, uint32_t ctr, const uint8_t *rk ) {
    static const uint32_t mask[4] = {0, 1, 2, 3};
    uint32x4_t idx   = vld1q_u32( mask );
    uint32x4_t ctrx4 = vdupq_n_u32( ctr );
    uint32x4_t ctrx4_be = vreinterpretq_u32_u8(vrev32q_u8( vreinterpretq_u8_u32(vaddq_u32(ctrx4, idx)) )); // to big endian numbers

    uint32x4_t iv = vreinterpretq_u32_u8(vld1q_u8(nonce));
    uint8x16_t p0 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 0 ));
    uint8x16_t p1 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 1 ));
    uint8x16_t p2 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 2 ));
    uint8x16_t p3 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 3 ));

    uint8x16_t rki;
    for (int i = 0; i < 9; i++) {
        rki = vld1q_u8(rk);
        rk += 16;
        p0 = vaeseq_u8( p0, rki );
        p1 = vaeseq_u8( p1, rki );
        p2 = vaeseq_u8( p2, rki );
        p3 = vaeseq_u8( p3, rki );
        p0 = vaesmcq_u8( p0 );
        p1 = vaesmcq_u8( p1 );
        p2 = vaesmcq_u8( p2 );
        p3 = vaesmcq_u8( p3 );
    }
    rki = vld1q_u8(rk);
    rk += 16;
    p0 = vaeseq_u8( p0, rki );
    p1 = vaeseq_u8( p1, rki );
    p2 = vaeseq_u8( p2, rki );
    p3 = vaeseq_u8( p3, rki );
    rki = vld1q_u8(rk);
    p0 ^= rki;
    p1 ^= rki;
    p2 ^= rki;
    p3 ^= rki;
    vst1q_u8( ct, p0 );
    vst1q_u8( ct + 16, p1 );
    vst1q_u8( ct + 32, p2 );
    vst1q_u8( ct + 48, p3 );
}

void aes128ctrx4_4r_enc_neonaes( uint8_t *ct, const uint8_t *nonce, uint32_t ctr, const uint8_t *rk ) {
    static const uint32_t mask[4] = {0, 1, 2, 3};
    uint32x4_t idx   = vld1q_u32( mask );
    uint32x4_t ctrx4 = vdupq_n_u32( ctr );
    uint32x4_t ctrx4_be = vreinterpretq_u32_u8(vrev32q_u8( vreinterpretq_u8_u32(vaddq_u32(ctrx4, idx)) )); // to big endian numbers

    uint32x4_t iv = vreinterpretq_u32_u8(vld1q_u8(nonce));
    uint8x16_t p0 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 0 ));
    uint8x16_t p1 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 1 ));
    uint8x16_t p2 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 2 ));
    uint8x16_t p3 = vreinterpretq_u8_u32(vcopyq_laneq_u32( iv, 3, ctrx4_be, 3 ));

    uint8x16_t rki;
    for (int i = 0; i < 3; i++) {
        rki = vld1q_u8(rk);
        rk += 16;
        p0 = vaeseq_u8( p0, rki );
        p1 = vaeseq_u8( p1, rki );
        p2 = vaeseq_u8( p2, rki );
        p3 = vaeseq_u8( p3, rki );
        p0 = vaesmcq_u8( p0 );
        p1 = vaesmcq_u8( p1 );
        p2 = vaesmcq_u8( p2 );
        p3 = vaesmcq_u8( p3 );
    }
    rki = vld1q_u8(rk);
    rk += 16;
    p0 = vaeseq_u8( p0, rki );
    p1 = vaeseq_u8( p1, rki );
    p2 = vaeseq_u8( p2, rki );
    p3 = vaeseq_u8( p3, rki );
    rki = vld1q_u8(rk);
    p0 ^= rki;
    p1 ^= rki;
    p2 ^= rki;
    p3 ^= rki;
    vst1q_u8( ct, p0 );
    vst1q_u8( ct + 16, p1 );
    vst1q_u8( ct + 32, p2 );
    vst1q_u8( ct + 48, p3 );
}

/////////////////////////////



void aes128_enc_neonaes( uint8_t *ct, const uint8_t *pk, const uint8_t *rk ) {
    uint8x16_t state = vld1q_u8(pk);
    for (int i = 0; i < 9; i++) {
        state = vaeseq_u8( state, vld1q_u8(rk) );
        rk += 16;
        state = vaesmcq_u8( state );
    }
    state = vaeseq_u8( state, vld1q_u8(rk) );
    rk += 16;
    state ^= vld1q_u8(rk);
    vst1q_u8( ct, state );
}

void aes128_4r_enc_neonaes( uint8_t *ct, const uint8_t *pk, const uint8_t *rk ) {
    uint8x16_t state = vld1q_u8(pk);
    for (int i = 0; i < 3; i++) {
        state = vaeseq_u8( state, vld1q_u8(rk) );
        rk += 16;
        state = vaesmcq_u8( state );
    }
    state = vaeseq_u8( state, vld1q_u8(rk) );
    rk += 16;
    state ^= vld1q_u8(rk);
    vst1q_u8( ct, state );
}


void aes256_enc_neonaes( uint8_t *ct, const uint8_t *pk, const uint8_t *rk ) {
    uint8x16_t state = vld1q_u8(pk);
    for (int i = 0; i < 13; i++) {
        state = vaeseq_u8( state, vld1q_u8(rk) );
        rk += 16;
        state = vaesmcq_u8( state );
    }
    state = vaeseq_u8( state, vld1q_u8(rk) );
    rk += 16;
    state ^= vld1q_u8(rk);
    vst1q_u8( ct, state );
}



////////////////////////////////




static uint8_t rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};


#define LE_LOAD_32(x)                                                                           \
    ((((uint32_t)((x)[3])) << 24) |                                             \
     (((uint32_t)((x)[2])) << 16) |                                             \
     (((uint32_t)((x)[1])) << 8) |                                                      \
     ((uint32_t)((x)[0])))


static inline
uint32_t sbox( uint32_t w ) {
    uint32x4_t ww = vdupq_n_u32( w );
    uint32x4_t w1 = vreinterpretq_u32_u8(vaeseq_u8( vreinterpretq_u8_u32(ww), vdupq_n_u8(0) ));
    return vgetq_lane_u32( w1, 0 );
}


static inline
uint32_t sbox_ror( uint32_t w ) {
    uint32x4_t ww = vdupq_n_u32( w );
    uint32x4_t w1 = vreinterpretq_u32_u8(vaeseq_u8( vreinterpretq_u8_u32(ww), vdupq_n_u8(0) ));
    uint32x4_t w2 = vsliq_n_u32( vshrq_n_u32(w1, 8), w1, 24 );
    return vgetq_lane_u32( w2, 0 );
}


#include "string.h"

// roundkeys: 11*16 bytes
void aes128_keyexp_neonaes( uint8_t *roundkeys, const uint8_t *key) {
    uint32_t rkeys[44];
    // key schedule in the classical representation
    rkeys[0] = LE_LOAD_32(key);
    rkeys[1] = LE_LOAD_32(key + 4);
    rkeys[2] = LE_LOAD_32(key + 8);
    rkeys[3] = LE_LOAD_32(key + 12);
    for (int i = 4; i < 44; i += 4) {
        rkeys[i] = rkeys[i - 4] ^ rcon[i / 4];
        rkeys[i] ^= sbox_ror( rkeys[i - 1] );
        rkeys[i + 1] = rkeys[i] ^ rkeys[i - 3];
        rkeys[i + 2] = rkeys[i + 1] ^ rkeys[i - 2];
        rkeys[i + 3] = rkeys[i + 2] ^ rkeys[i - 1];
    }
    memcpy( roundkeys, (uint8_t *)rkeys, 11 * 16 );
}


// roundkeys: 5*16 bytes
void aes128_4r_keyexp_neonaes( uint8_t *roundkeys, const uint8_t *key) {
    uint32_t rkeys[20];
    // key schedule in the classical representation
    rkeys[0] = LE_LOAD_32(key);
    rkeys[1] = LE_LOAD_32(key + 4);
    rkeys[2] = LE_LOAD_32(key + 8);
    rkeys[3] = LE_LOAD_32(key + 12);
    for (int i = 4; i < 20; i += 4) {
        rkeys[i] = rkeys[i - 4] ^ rcon[i / 4];
        rkeys[i] ^= sbox_ror( rkeys[i - 1] );
        rkeys[i + 1] = rkeys[i] ^ rkeys[i - 3];
        rkeys[i + 2] = rkeys[i + 1] ^ rkeys[i - 2];
        rkeys[i + 3] = rkeys[i + 2] ^ rkeys[i - 1];
    }
    memcpy( roundkeys, (uint8_t *)rkeys, 5 * 16 );
}


// roundkeys: 15*16 bytes
void aes256_keyexp_neonaes( uint8_t *roundkeys, const uint8_t *key) {
    uint32_t rkeys[60];
    // key schedule in the classical representation
    rkeys[0] = LE_LOAD_32(key);
    rkeys[1] = LE_LOAD_32(key + 4);
    rkeys[2] = LE_LOAD_32(key + 8);
    rkeys[3] = LE_LOAD_32(key + 12);
    rkeys[4] = LE_LOAD_32(key + 16);
    rkeys[5] = LE_LOAD_32(key + 20);
    rkeys[6] = LE_LOAD_32(key + 24);
    rkeys[7] = LE_LOAD_32(key + 28);
    for (int i = 8; i < 56; i += 8) {
        rkeys[i] = rkeys[i - 8] ^ rcon[i / 8];
        rkeys[i] ^= sbox_ror( rkeys[i - 1] );
        rkeys[i + 1] = rkeys[i] ^ rkeys[i - 7];
        rkeys[i + 2] = rkeys[i + 1] ^ rkeys[i - 6];
        rkeys[i + 3] = rkeys[i + 2] ^ rkeys[i - 5];
        rkeys[i + 4] = rkeys[i - 4];
        rkeys[i + 4] ^= sbox(rkeys[i + 3]);
        rkeys[i + 5] = rkeys[i + 4] ^ rkeys[i - 3];
        rkeys[i + 6] = rkeys[i + 5] ^ rkeys[i - 2];
        rkeys[i + 7] = rkeys[i + 6] ^ rkeys[i - 1];
    }
    rkeys[56] = rkeys[48] ^ rcon[7];
    rkeys[56] ^= sbox_ror(rkeys[55]);
    rkeys[57] = rkeys[56] ^ rkeys[49];
    rkeys[58] = rkeys[57] ^ rkeys[50];
    rkeys[59] = rkeys[58] ^ rkeys[51];
    memcpy( roundkeys, (uint8_t *)rkeys, 15 * 16 );
}





