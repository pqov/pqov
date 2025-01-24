
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "benchmark.h"

#include "blas_neon.h"


static
uint8_t check_neq( const uint8_t * tab0 , const uint8_t * tab1 , unsigned size )
{
    uint8_t r = 0;
    for(unsigned i=0;i<size;i++) r |= (tab0[i]^tab1[i]);
    return r;
}



static inline
void gf256v_madd_neon_0( uint8_t *accu_c, const uint8_t *a, uint8_t b, unsigned _num_byte ) {
    uint8x16_t bp = vdupq_n_u8(b);
    #if defined(_GF256_REDUCE_WITH_TBL_)
    uint8x16_t mask_f = vdupq_n_u8( 0xf );
    uint8x16_t tab_rd0 = vld1q_u8(__gf256_bit8_11_reduce);
    uint8x16_t tab_rd1 = vld1q_u8(__gf256_bit12_15_reduce);
    #else
    uint8x16_t mask_0x1b = vdupq_n_u8( 0x1b );
    uint8x16_t mask_3 = vdupq_n_u8( 3 );
    #endif
    while ( _num_byte >= 48 ) {
        uint8x16_t a0 = vld1q_u8(a);
        uint8x16_t a1 = vld1q_u8(a + 16);
        uint8x16_t a2 = vld1q_u8(a + 32);
        uint8x16_t c0 = vld1q_u8(accu_c);
        uint8x16_t c1 = vld1q_u8(accu_c + 16);
        uint8x16_t c2 = vld1q_u8(accu_c + 32);
        #if defined(_GF256_REDUCE_WITH_TBL_)
        vst1q_u8( accu_c, c0 ^ _gf256v_mul_neon(a0, bp, mask_f, tab_rd0, tab_rd1) );
        vst1q_u8( accu_c+16, c1 ^ _gf256v_mul_neon(a1, bp, mask_f, tab_rd0, tab_rd1) );
        vst1q_u8( accu_c+32, c2 ^ _gf256v_mul_neon(a2, bp, mask_f, tab_rd0, tab_rd1) );
        #else
        vst1q_u8( accu_c, c0 ^ _gf256v_mul_neon(a0, bp, mask_3, mask_0x1b) );
        vst1q_u8( accu_c+16, c1 ^ _gf256v_mul_neon(a1, bp, mask_3, mask_0x1b) );
        vst1q_u8( accu_c+32, c2 ^ _gf256v_mul_neon(a2, bp, mask_3, mask_0x1b) );
        #endif
        _num_byte -= 48;
        a += 48;
        accu_c += 48;
    }
    while ( _num_byte >= 16 ) {
        uint8x16_t aa = vld1q_u8(a);
        uint8x16_t cc = vld1q_u8(accu_c);
        #if defined(_GF256_REDUCE_WITH_TBL_)
        vst1q_u8( accu_c, cc ^ _gf256v_mul_neon(aa, bp, mask_f, tab_rd0, tab_rd1) );
        #else
        vst1q_u8( accu_c, cc ^ _gf256v_mul_neon(aa, bp, mask_3, mask_0x1b) );
        #endif
        _num_byte -= 16;
        a += 16;
        accu_c += 16;
    }
    if (_num_byte ){
        uint8x16_t aa = _load_Qreg(a, _num_byte);
        uint8x16_t cc = _load_Qreg(accu_c, _num_byte);
        #if defined(_GF256_REDUCE_WITH_TBL_)
        uint8x16_t bb = _gf256v_mul_neon(aa, bp, mask_f, tab_rd0, tab_rd1);            
        #else
        uint8x16_t bb = _gf256v_mul_neon(aa, bp, mask_3, mask_0x1b);
        #endif
        _store_Qreg( accu_c, _num_byte, bb ^ cc );
    }
}


static inline
void gf256v_madd_neon_1( uint8_t *accu_c, const uint8_t *a, uint8_t b, unsigned _num_byte ) {
    uint8x16_t bp = vdupq_n_u8(b);
    #if defined(_GF256_REDUCE_WITH_TBL_)
    uint8x16_t mask_f = vdupq_n_u8( 0xf );
    uint8x16_t tab_rd0 = vld1q_u8(__gf256_bit8_11_reduce);
    uint8x16_t tab_rd1 = vld1q_u8(__gf256_bit12_15_reduce);
    #else
    uint8x16_t mask_0x1b = vdupq_n_u8( 0x1b );
    uint8x16_t mask_3 = vdupq_n_u8( 3 );
    #endif
    if (_num_byte) {
        if (_num_byte < 16 ){
            uint8x16_t aa = _load_Qreg(a, _num_byte);
            uint8x16_t cc = _load_Qreg(accu_c, _num_byte);
            #if defined(_GF256_REDUCE_WITH_TBL_)
            uint8x16_t bb = _gf256v_mul_neon(aa, bp, mask_f, tab_rd0, tab_rd1);
            #else
            uint8x16_t bb = _gf256v_mul_neon(aa, bp, mask_3, mask_0x1b);
            #endif
            _store_Qreg( accu_c, _num_byte, bb ^ cc );
            return;
        } else {
            unsigned len = _num_byte & 15;
            uint8x16_t aa = vld1q_u8(a);
            uint8x16_t cc = vld1q_u8(accu_c);
            uint8x16_t c1 = vld1q_u8(accu_c+len);
            #if defined(_GF256_REDUCE_WITH_TBL_)
            uint8x16_t bb = _gf256v_mul_neon(aa, bp, mask_f, tab_rd0, tab_rd1);
            #else
            uint8x16_t bb = _gf256v_mul_neon(aa, bp, mask_3, mask_0x1b);
            #endif
            vst1q_u8( accu_c, bb ^ cc );
            vst1q_u8( accu_c+len , c1 );
            a += len;
            accu_c += len;
            _num_byte -= len;
        }
    }
    while ( _num_byte >= 48 ) {
        uint8x16_t a0 = vld1q_u8(a);
        uint8x16_t a1 = vld1q_u8(a + 16);
        uint8x16_t a2 = vld1q_u8(a + 32);
        uint8x16_t c0 = vld1q_u8(accu_c);
        uint8x16_t c1 = vld1q_u8(accu_c + 16);
        uint8x16_t c2 = vld1q_u8(accu_c + 32);
        #if defined(_GF256_REDUCE_WITH_TBL_)
        vst1q_u8( accu_c, c0 ^ _gf256v_mul_neon(a0, bp, mask_f, tab_rd0, tab_rd1) );
        vst1q_u8( accu_c+16, c1 ^ _gf256v_mul_neon(a1, bp, mask_f, tab_rd0, tab_rd1) );
        vst1q_u8( accu_c+32, c2 ^ _gf256v_mul_neon(a2, bp, mask_f, tab_rd0, tab_rd1) );
        #else
        vst1q_u8( accu_c, c0 ^ _gf256v_mul_neon(a0, bp, mask_3, mask_0x1b) );
        vst1q_u8( accu_c+16, c1 ^ _gf256v_mul_neon(a1, bp, mask_3, mask_0x1b) );
        vst1q_u8( accu_c+32, c2 ^ _gf256v_mul_neon(a2, bp, mask_3, mask_0x1b) );
        #endif
        _num_byte -= 48;
        a += 48;
        accu_c += 48;
    }
    while ( _num_byte ) {
        uint8x16_t aa = vld1q_u8(a);
        uint8x16_t cc = vld1q_u8(accu_c);
        #if defined(_GF256_REDUCE_WITH_TBL_)
        vst1q_u8( accu_c, cc ^ _gf256v_mul_neon(aa, bp, mask_f, tab_rd0, tab_rd1) );
        #else
        vst1q_u8( accu_c, cc ^ _gf256v_mul_neon(aa, bp, mask_3, mask_0x1b) );
        #endif
        _num_byte -= 16;
        a += 16;
        accu_c += 16;
    }
}



static inline
void _gf256v_madd_multab_neon2( uint8_t *accu_c, const uint8_t *a,
                               uint8x16_t tbl0, uint8x16_t tbl1, unsigned _num_byte, uint8x16_t mask_f ) {
    if (15 & _num_byte) {
        unsigned len = 15 & _num_byte;
        if ( _num_byte < 16 ) {
            uint8x16_t aa = _load_Qreg(a,len);
            uint8x16_t cc = _load_Qreg(accu_c,len);
            _store_Qreg( accu_c, len, cc ^ _gf256_tbl(aa, tbl0, tbl1, mask_f) );
            return;
        } else {
            uint8x16_t aa = vld1q_u8(a);
            uint8x16_t cc = vld1q_u8(accu_c);
            uint8x16_t c1 = vld1q_u8(accu_c+len);
            vst1q_u8( accu_c, cc ^ _gf256_tbl(aa, tbl0, tbl1, mask_f) );
            vst1q_u8( accu_c + len , c1 );
            _num_byte -= len;
            accu_c += len;
            a += len;
        }
    }
    while ( _num_byte >= 48 ) {
        uint8x16_t a0 = vld1q_u8(a);
        uint8x16_t a1 = vld1q_u8(a + 16);
        uint8x16_t a2 = vld1q_u8(a + 32);
        uint8x16_t c0 = vld1q_u8(accu_c);
        uint8x16_t c1 = vld1q_u8(accu_c + 16);
        uint8x16_t c2 = vld1q_u8(accu_c + 32);
        vst1q_u8( accu_c, c0 ^ _gf256_tbl(a0, tbl0, tbl1, mask_f) );
        vst1q_u8( accu_c + 16, c1 ^ _gf256_tbl(a1, tbl0, tbl1, mask_f) );
        vst1q_u8( accu_c + 32, c2 ^ _gf256_tbl(a2, tbl0, tbl1, mask_f) );
        _num_byte -= 48;
        a += 48;
        accu_c += 48;
    }
    while ( _num_byte ) {
        uint8x16_t aa = vld1q_u8(a);
        uint8x16_t cc = vld1q_u8(accu_c);
        vst1q_u8( accu_c, cc ^ _gf256_tbl(aa, tbl0, tbl1, mask_f) );
        _num_byte -= 16;
        a += 16;
        accu_c += 16;
    }
}

static inline
void gf256v_madd_neon_2( uint8_t *accu_c, const uint8_t *a, uint8_t b, unsigned _num_byte ) {
    uint8x16_t mask_f = vdupq_n_u8( 0xf );
    uint8x16x2_t t = gf256v_get_multab_neon(b);
    uint8x16_t tbl0 = t.val[0];
    uint8x16_t tbl1 = t.val[1];
    _gf256v_madd_multab_neon2( accu_c, a, tbl0, tbl1, _num_byte, mask_f );
}



#define TEST_RUN (10000)
#define BUF_LEN  (4096)

uint8_t buffer0[BUF_LEN] __attribute__((aligned(32)));
uint8_t buffer1[BUF_LEN] __attribute__((aligned(32)));
uint8_t buffer2[BUF_LEN] __attribute__((aligned(32)));
uint8_t buffer3[BUF_LEN] __attribute__((aligned(32)));
uint8_t buffer4[BUF_LEN] __attribute__((aligned(32)));

static
int test_vec_madd(unsigned vec_len)
{
    printf("Benchmark vector madd of length %d:\n", vec_len );

    struct benchmark bm0, bm1, bm2;
    bm_init(&bm0);
    bm_init(&bm1);
    bm_init(&bm2);

    char msg[256];
    for (unsigned i = 0; i < 256; i++) {
        msg[i] = i;
    }

    for (unsigned i=0;i<sizeof(buffer0);i++) {
        buffer0[i] = i*13;
        buffer1[i] = buffer0[i];
        buffer2[i] = buffer0[i];
        buffer3[i] = (i+47)*17;
        buffer4[i] = buffer0[i];
    }

    unsigned j = 0;
    for (unsigned i = 0; i < TEST_RUN; i++) {
        if (j+vec_len >= BUF_LEN ) { j = (i&7); }
        uint8_t *ptr0 = buffer0 + j;
        uint8_t *ptr1 = buffer1 + j;
        uint8_t *ptr2 = buffer2 + j;
        uint8_t *ptr3 = buffer3 + j;
        j += vec_len;

        BENCHMARK(bm0, {
            gf256v_madd_neon_0( ptr0 , ptr3 , i , vec_len );
        });
        BENCHMARK(bm1, {
            gf256v_madd_neon_1( ptr1 , ptr3 , i , vec_len );
        });
        BENCHMARK(bm2, {
            gf256v_madd_neon_2( ptr2 , ptr3 , i , vec_len );
        });

        if ( check_neq( ptr0, ptr1 , vec_len ) ) {
            printf("!!! Tables NEQ: 01 [%d].\n", i);
            return -1;
        }
        if ( check_neq( ptr0, ptr2 , vec_len ) ) {
            printf("!!! Tables NEQ: 02 [%d].\n", i);
            return -1;
        }
    }
    bm_dump(msg, 256, &bm0);
    printf("benchmark: neon vec madd 0: %s\n", msg );
    bm_dump(msg, 256, &bm1);
    printf("benchmark: neon vec madd 1: %s\n", msg );
    bm_dump(msg, 256, &bm2);
    printf("benchmark: neon vec madd 2: %s\n", msg );
    return 0;
}



int main(void) {

    test_vec_madd( 32 );
    test_vec_madd( 40 );
    test_vec_madd( 60 );
    test_vec_madd( 80 );
    test_vec_madd( 96 );
    test_vec_madd( 144 );
    return 0;
}

