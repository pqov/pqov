// code from https://www.intel.com/content/dam/doc/white-paper/advanced-encryption-standard-new-instructions-set-paper.pdf
//
//

#include "x86aesni.h"
#include <wmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

// AES 256 key-expansion

static inline void KEY_256_ASSIST_1(__m128i *temp1, __m128i *temp2) {
    __m128i temp4;
    *temp2 = _mm_shuffle_epi32(*temp2, 0xff);
    temp4 = _mm_slli_si128 (*temp1, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    *temp1 = _mm_xor_si128 (*temp1, *temp2);
}
static inline void KEY_256_ASSIST_2(__m128i *temp1, __m128i *temp3) {
    __m128i temp2, temp4;
    temp4 = _mm_aeskeygenassist_si128 (*temp1, 0x0);
    temp2 = _mm_shuffle_epi32(temp4, 0xaa);
    temp4 = _mm_slli_si128 (*temp3, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    *temp3 = _mm_xor_si128 (*temp3, temp2);
}
void AES256_Key_Expansion (unsigned char *key, const unsigned char *userkey) {
    __m128i temp1, temp2, temp3;
    __m128i *Key_Schedule = (__m128i *)key;
    temp1 = _mm_loadu_si128((__m128i *)userkey);
    temp3 = _mm_loadu_si128((__m128i *)(userkey + 16));
    Key_Schedule[0] = temp1;
    Key_Schedule[1] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3, 0x01);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[2] = temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[3] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3, 0x02);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[4] = temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[5] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3, 0x04);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[6] = temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[7] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3, 0x08);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[8] = temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[9] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3, 0x10);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[10] = temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[11] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3, 0x20);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[12] = temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[13] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3, 0x40);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[14] = temp1;
}



// AES256 CTR

#define _AES256_NUM_ROUNDS  14

void AES256_CTR_Encrypt ( unsigned char *out, unsigned long n_16B, const unsigned char *key, const unsigned char nonce[16], uint32_t ctr ) {
    __m128i ctr_block, tmp, ONE, BSWAP;
    int i, j;
    int length = n_16B;
    ONE   = _mm_set_epi32(0, 0, 0, 1);
    BSWAP = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    ctr_block = _mm_loadu_si128( (__m128i const *)nonce ); // nonce is a 128-bit number in big endian
    ctr_block = _mm_shuffle_epi8(ctr_block, BSWAP);        // change endian:  big -> small
    ctr_block = _mm_insert_epi32(ctr_block, ctr, 0);       // ctr is a small endia number
    for (i = 0; i < length; i++) {
        tmp = _mm_shuffle_epi8(ctr_block, BSWAP);    // change endian:  small -> big
        ctr_block = _mm_add_epi64(ctr_block, ONE);   // the addition only work for small endian data
        tmp = _mm_xor_si128(tmp, ((__m128i *)key)[0]);
        for (j = 1; j < _AES256_NUM_ROUNDS; j++) {
            tmp = _mm_aesenc_si128 (tmp, ((__m128i *)key)[j]);
        };
        tmp = _mm_aesenclast_si128 (tmp, ((__m128i *)key)[j]);
        //tmp = _mm_xor_si128(tmp,_mm_loadu_si128(&((__m128i*)in)[i]));
        _mm_storeu_si128 (&((__m128i *)out)[i], tmp);
    }
}




////////////////////////////////////////////////////////////////////////////////////


static inline __m128i AES_128_ASSIST (__m128i temp1, __m128i temp2) {
    __m128i temp3;
    temp2 = _mm_shuffle_epi32 (temp2, 0xff);
    temp3 = _mm_slli_si128 (temp1, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp1 = _mm_xor_si128 (temp1, temp2);
    return temp1;
}


void AES128_Key_Expansion (unsigned char *key, const unsigned char *userkey) {
    __m128i temp1, temp2;
    __m128i *Key_Schedule = (__m128i *)key;

    temp1 = _mm_loadu_si128((__m128i *)userkey);
    Key_Schedule[0] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x1);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[1] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x2);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[2] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x4);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[3] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x8);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[4] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x10);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[5] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x20);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[6] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x40);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[7] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x80);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[8] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x1b);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[9] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x36);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[10] = temp1;
}


// AES128 CTR

#define _AES128_NUM_ROUNDS  10

void AES128_CTR_Encrypt ( unsigned char *out, unsigned long n_16B, const unsigned char *key, const unsigned char nonce[16], uint32_t ctr ) {
    __m128i ctr_block, tmp, ONE, BSWAP;
    int i, j;
    int length = n_16B;
    ONE   = _mm_set_epi32(0, 0, 0, 1);
    BSWAP = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    ctr_block = _mm_loadu_si128( (__m128i const *)nonce ); // nonce is a 128-bit number in big endian
    ctr_block = _mm_shuffle_epi8(ctr_block, BSWAP);        // change endian:  big -> small
    ctr_block = _mm_insert_epi32(ctr_block, ctr, 0);       // ctr is a small endia number
    for (i = 0; i < length; i++) {
        tmp = _mm_shuffle_epi8(ctr_block, BSWAP);    // change endian:  small -> big
        ctr_block = _mm_add_epi64(ctr_block, ONE);   // the addition only work for small endian data
        tmp = _mm_xor_si128(tmp, ((__m128i *)key)[0]);
        for (j = 1; j < _AES128_NUM_ROUNDS; j++) {
            tmp = _mm_aesenc_si128 (tmp, ((__m128i *)key)[j]);
        };
        tmp = _mm_aesenclast_si128 (tmp, ((__m128i *)key)[j]);
        //tmp = _mm_xor_si128(tmp,_mm_loadu_si128(&((__m128i*)in)[i]));
        _mm_storeu_si128 (&((__m128i *)out)[i], tmp);
    }
}


////////////////////////////////////////////////////////////////////////////////////





void AES128_4R_Key_Expansion (unsigned char *key, const unsigned char *userkey) {
    __m128i temp1, temp2;
    __m128i *Key_Schedule = (__m128i *)key;

    temp1 = _mm_loadu_si128((__m128i *)userkey);
    Key_Schedule[0] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x1);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[1] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x2);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[2] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x4);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[3] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1, 0x8);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[4] = temp1;
//temp2 = _mm_aeskeygenassist_si128 (temp1,0x10);
//temp1 = AES_128_ASSIST(temp1, temp2);
//Key_Schedule[5] = temp1;
}



#define _AES128_4R_NUM_ROUNDS  4

void AES128_4R_CTR_Encrypt ( unsigned char *out, unsigned long n_16B, const unsigned char *key, const unsigned char nonce[16], uint32_t ctr ) {
    __m128i ctr_block, tmp, ONE, BSWAP;
    int i, j;
    int length = n_16B;
    ONE   = _mm_set_epi32(0, 0, 0, 1);
    BSWAP = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    ctr_block = _mm_loadu_si128( (__m128i const *)nonce ); // nonce is a 128-bit number in big endian
    ctr_block = _mm_shuffle_epi8(ctr_block, BSWAP);        // change endian:  big -> small
    ctr_block = _mm_insert_epi32(ctr_block, ctr, 0);       // ctr is a small endia number
    for (i = 0; i < length; i++) {
        tmp = _mm_shuffle_epi8(ctr_block, BSWAP);    // change endian:  small -> big
        ctr_block = _mm_add_epi64(ctr_block, ONE);   // the addition only work for small endian data
        tmp = _mm_xor_si128(tmp, ((__m128i *)key)[0]);
        for (j = 1; j < _AES128_4R_NUM_ROUNDS; j++) {
            tmp = _mm_aesenc_si128 (tmp, ((__m128i *)key)[j]);
        };
        tmp = _mm_aesenclast_si128 (tmp, ((__m128i *)key)[j]);
        //tmp = _mm_xor_si128(tmp,_mm_loadu_si128(&((__m128i*)in)[i]));
        _mm_storeu_si128 (&((__m128i *)out)[i], tmp);
    }
}

