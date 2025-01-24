
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "benchmark.h"

#include "blas_avx2.h"


static
uint8_t check_neq( const uint8_t * tab0 , const uint8_t * tab1 , unsigned size )
{
    uint8_t r = 0;
    for(unsigned i=0;i<size;i++) r |= (tab0[i]^tab1[i]);
    return r;
}


static inline
void linearmap_8x8_accu_ymm0( uint8_t *accu_c, const uint8_t *a,  __m256i ml, __m256i mh, __m256i mask, unsigned _num_byte ) {
    unsigned n_32 = _num_byte >> 5;
    unsigned rem = _num_byte & 31;
    while (n_32--) {
        __m256i inp = _mm256_loadu_si256( (__m256i *)a );
        __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
        __m256i r0 = out ^ linear_transform_8x8_256b( ml, mh, inp, mask );
        _mm256_storeu_si256( (__m256i *)accu_c, r0 );
        a += 32;
        accu_c += 32;
    }
    if (rem ) {
        __m256i inp = _load_ymm( a , rem);
        __m256i out = _load_ymm( accu_c , rem);
        __m256i r0 = out ^ linear_transform_8x8_256b( ml, mh, inp, mask );
        _store_ymm( accu_c , rem , r0 );
    }
}

static inline
void gf256v_madd_avx2_0( uint8_t *accu_c, const uint8_t *a, uint8_t _b, unsigned _num_byte ) {
    __m256i m_tab = tbl32_gf256_multab( _b );
    __m256i ml = _mm256_permute2x128_si256( m_tab, m_tab, 0 );
    __m256i mh = _mm256_permute2x128_si256( m_tab, m_tab, 0x11 );
    __m256i mask = _mm256_set1_epi8(0xf);

    linearmap_8x8_accu_ymm0( accu_c, a, ml, mh, mask, _num_byte );
}


static inline
void linearmap_8x8_accu_ymm1( uint8_t *accu_c, const uint8_t *a,  __m256i ml, __m256i mh, __m256i mask, unsigned _num_byte ) {
    unsigned n_32 = _num_byte >> 5;
    unsigned rem = _num_byte & 31;
    if ( rem ) {
        if (n_32 ) {
            __m256i indices = _mm256_set_epi32( 0x1f1e1d1c, 0x1b1a1918, 0x17161514, 0x13121110, 0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100 );
            __m256i meff = _mm256_cmpgt_epi8( _mm256_set1_epi8(rem), indices );
            __m256i inp = _mm256_loadu_si256((__m256i *)a);
            __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
            __m256i r0 = out ^ (linear_transform_8x8_256b( ml, mh, inp, mask )&meff);
            _mm256_storeu_si256( (__m256i *)accu_c, r0 );
        } else {
            linearmap_8x8_accu_sse( accu_c, a, _mm256_castsi256_si128(ml), _mm256_castsi256_si128(mh), _mm256_castsi256_si128(mask), rem );
        }
        a += rem;
        accu_c += rem;
    }
    while (n_32--) {
        __m256i inp = _mm256_loadu_si256( (__m256i *)a );
        __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
        __m256i r0 = out ^ linear_transform_8x8_256b( ml, mh, inp, mask );
        _mm256_storeu_si256( (__m256i *)accu_c, r0 );
        a += 32;
        accu_c += 32;
    }
}

static inline
void gf256v_madd_avx2_1( uint8_t *accu_c, const uint8_t *a, uint8_t _b, unsigned _num_byte ) {
    __m256i m_tab = tbl32_gf256_multab( _b );
    __m256i ml = _mm256_permute2x128_si256( m_tab, m_tab, 0 );
    __m256i mh = _mm256_permute2x128_si256( m_tab, m_tab, 0x11 );
    __m256i mask = _mm256_set1_epi8(0xf);

    linearmap_8x8_accu_ymm1( accu_c, a, ml, mh, mask, _num_byte );
}

static inline
void linearmap_8x8_accu_ymm2( uint8_t *accu_c, const uint8_t *a,  __m256i ml, __m256i mh, __m256i mask, unsigned _num_byte ) {
    unsigned n_32 = _num_byte >> 5;
    unsigned rem = _num_byte & 31;
    if ( rem ) {
        if (n_32 ) {
            __m256i inp = _mm256_loadu_si256( (__m256i *)a );
            __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
            __m256i ou1 = _mm256_loadu_si256( (__m256i *)(accu_c+rem) );
            __m256i r0 = out ^ linear_transform_8x8_256b( ml, mh, inp, mask );
            _mm256_storeu_si256( (__m256i *)accu_c, r0 );
            _mm256_storeu_si256( (__m256i *)(accu_c+rem), ou1 );
        } else {
            linearmap_8x8_accu_sse( accu_c, a, _mm256_castsi256_si128(ml), _mm256_castsi256_si128(mh), _mm256_castsi256_si128(mask), rem );
        }
        a += rem;
        accu_c += rem;
    }
    while (n_32--) {
        __m256i inp = _mm256_loadu_si256( (__m256i *)a );
        __m256i out = _mm256_loadu_si256( (__m256i *)accu_c );
        __m256i r0 = out ^ linear_transform_8x8_256b( ml, mh, inp, mask );
        _mm256_storeu_si256( (__m256i *)accu_c, r0 );
        a += 32;
        accu_c += 32;
    }
}

static inline
void gf256v_madd_avx2_2( uint8_t *accu_c, const uint8_t *a, uint8_t _b, unsigned _num_byte ) {
    __m256i m_tab = tbl32_gf256_multab( _b );
    __m256i ml = _mm256_permute2x128_si256( m_tab, m_tab, 0 );
    __m256i mh = _mm256_permute2x128_si256( m_tab, m_tab, 0x11 );
    __m256i mask = _mm256_set1_epi8(0xf);

    linearmap_8x8_accu_ymm2( accu_c, a, ml, mh, mask, _num_byte );
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

        //gf256v_madd_avx2_0( buffer4 , ptr3 , i , vec_len );

        BENCHMARK(bm0, {
            gf256v_madd_avx2_0( ptr0 , ptr3 , i , vec_len );
        });
        BENCHMARK(bm1, {
            gf256v_madd_avx2_1( ptr1 , ptr3 , i , vec_len );
        });
        BENCHMARK(bm2, {
            gf256v_madd_avx2_2( ptr2 , ptr3 , i , vec_len );
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
    printf("benchmark: avx2 vec madd 0: %s\n", msg );
    bm_dump(msg, 256, &bm1);
    printf("benchmark: avx2 vec madd 1: %s\n", msg );
    bm_dump(msg, 256, &bm2);
    printf("benchmark: avx2 vec madd 2: %s\n", msg );
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

