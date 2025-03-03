// SPDX-License-Identifier: CC0 OR Apache-2.0
/// @file utils_malloc.h
/// @brief the interface for adapting malloc functions.
///
///

#ifndef _UTILS_MALLOC_H_
#define _UTILS_MALLOC_H_


#include <stdlib.h>


#if defined(__GNUC__) || defined(__clang__)
#define PQOV_ALIGN  __attribute__((aligned(32)))
#elif defined(_MSC_VER)
#define PQOV_ALIGN  __declspec(align(32))
#else
#define PQOV_ALIGN
#endif


#if !defined(PQM4)
#define _HAS_MEMALIGN_
#endif


#ifdef  __cplusplus
extern  "C" {
#endif


static inline
void *adapted_alloc( size_t alignment, size_t size ) {
    #if defined(MEMALIGN)
    return memalign( alignment, size );
    #else
    (void)(alignment);
    return malloc( size );
    #endif
}



#ifdef  __cplusplus
}
#endif



#endif // _UTILS_MALLOC_H_


