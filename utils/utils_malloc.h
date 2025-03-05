// SPDX-License-Identifier: CC0 OR Apache-2.0
/// @file utils_malloc.h
/// @brief the interface for adapting malloc functions.
///
///

#ifndef _UTILS_MALLOC_H_
#define _UTILS_MALLOC_H_

#include <stdlib.h>

#define ov_malloc malloc

static inline void ov_free(void *ptr, size_t len){
    (void) len;
    free(ptr);
}

#if !defined(PQM4)
#define _HAS_MEMALIGN_
#endif

#if defined(__GNUC__) || defined(__clang__)
#define PQOV_ALIGN  __attribute__((aligned(32)))
#elif defined(_MSC_VER)
#define PQOV_ALIGN  __declspec(align(32))
#else
#define PQOV_ALIGN
#endif

#endif // _UTILS_MALLOC_H_


