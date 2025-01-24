
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
void gf256v_add_avx2_0( uint8_t *accu_b, const uint8_t *a, unsigned _num_byte ) {
    while ( _num_byte >= 32 ) {
        _mm256_storeu_si256( (__m256i *)accu_b, _mm256_loadu_si256((__m256i *)a) ^ _mm256_loadu_si256((__m256i *)accu_b) );
        accu_b += 32;
        a += 32;
        _num_byte -= 32;
    }
    if (_num_byte) {
        __m256i aa = _load_ymm(a,_num_byte);
        __m256i bb = _load_ymm(accu_b,_num_byte);
        _store_ymm(accu_b,_num_byte,bb^aa);
    }
}



static inline
void gf256v_add_avx2_1( uint8_t *accu_b, const uint8_t *a, unsigned _num_byte ) {
    if ( _num_byte & 31 ) {
        unsigned rem = _num_byte & 31;
        if ( _num_byte < 32 ) {
            gf256v_add_sse( accu_b, a, _num_byte );
        } else {
            __m256i indices = _mm256_set_epi32( 0x1f1e1d1c, 0x1b1a1918, 0x17161514, 0x13121110, 0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100 );
            __m256i mask = _mm256_cmpgt_epi8( _mm256_set1_epi8(rem), indices );
            __m256i aa = _mm256_loadu_si256((__m256i *)a);
            __m256i ab = _mm256_loadu_si256((__m256i *)accu_b);
            __m256i bb = ab ^ (aa&mask);
            _mm256_storeu_si256( (__m256i *)accu_b, bb );
        }
        a += rem;
        accu_b += rem;
        _num_byte -= rem;
    }
    while ( _num_byte ) {
        _mm256_storeu_si256( (__m256i *)accu_b, _mm256_loadu_si256((__m256i *)a) ^ _mm256_loadu_si256((__m256i *)accu_b) );
        accu_b += 32;
        a += 32;
        _num_byte -= 32;
    }
}


static inline
void gf256v_add_avx2_2( uint8_t *accu_b, const uint8_t *a, unsigned _num_byte ) {
    if ( _num_byte & 31 ) {
        unsigned rem = _num_byte & 31;
        if ( _num_byte < 32 ) {
            gf256v_add_sse( accu_b, a, _num_byte );
        } else {
            __m256i aa = _mm256_loadu_si256((__m256i *)a);
            __m256i ab = _mm256_loadu_si256((__m256i *)accu_b);
            __m256i ab1 = _mm256_loadu_si256((__m256i *)(accu_b+rem));
            _mm256_storeu_si256( (__m256i *)accu_b, aa^ab );
            _mm256_storeu_si256( (__m256i *)(accu_b+rem), ab1 );
        }
        a += rem;
        accu_b += rem;
        _num_byte -= rem;
    }
    while ( _num_byte ) {
        _mm256_storeu_si256( (__m256i *)accu_b, _mm256_loadu_si256((__m256i *)a) ^ _mm256_loadu_si256((__m256i *)accu_b) );
        accu_b += 32;
        a += 32;
        _num_byte -= 32;
    }
}

#define TEST_RUN (10000)
#define BUF_LEN  (4096)

uint8_t buffer0[BUF_LEN] __attribute__((aligned(32)));
uint8_t buffer1[BUF_LEN] __attribute__((aligned(32)));
uint8_t buffer2[BUF_LEN] __attribute__((aligned(32)));
uint8_t buffer3[BUF_LEN] __attribute__((aligned(32)));

static
int test_vec_add(unsigned vec_len)
{
    printf("Benchmark vector add of length %d:\n", vec_len );

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
            gf256v_add_avx2_0( ptr0 , ptr3 , vec_len );
        });
        BENCHMARK(bm1, {
            gf256v_add_avx2_1( ptr1 , ptr3 , vec_len );
        });
        BENCHMARK(bm2, {
            gf256v_add_avx2_2( ptr2 , ptr3 , vec_len );
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
    printf("benchmark: avx2 vec add 0: %s\n", msg );
    bm_dump(msg, 256, &bm1);
    printf("benchmark: avx2 vec add 1: %s\n", msg );
    bm_dump(msg, 256, &bm2);
    printf("benchmark: avx2 vec add 2: %s\n", msg );
    return 0;
}



int main(void) {

    test_vec_add( 32 );
    test_vec_add( 40 );
    test_vec_add( 60 );
    test_vec_add( 80 );
    test_vec_add( 96 );
    test_vec_add( 144 );
    return 0;
}

