#!/usr/bin/python

import sys
import os, errno, shutil
from shutil import copyfile

#
# HOW TO USE:
# ls -alR | python3 this_file
#

file_descriptions = {
"./Optimized_Implementation/amd64/III:":	"",
"./Optimized_Implementation/amd64/III_pkc:":	"",
"./Optimized_Implementation/amd64/III_pkc_skc:":	"",
"./Optimized_Implementation/amd64/Ip:":	"",
"./Optimized_Implementation/amd64/Ip_pkc:":	"",
"./Optimized_Implementation/amd64/Ip_pkc_skc:":	"",
"./Optimized_Implementation/amd64/Is:":	"",
"./Optimized_Implementation/amd64/Is_pkc:":	"",
"./Optimized_Implementation/amd64/Is_pkc_skc:":	"",
"./Optimized_Implementation/amd64/V:":	"",
"./Optimized_Implementation/amd64/V_pkc:":	"",
"./Optimized_Implementation/amd64/V_pkc_skc:":	"",
"./Optimized_Implementation/amd64/nistkat:":	"",
"./Optimized_Implementation/amd64:":	"",
"./Optimized_Implementation/avx2/III:":	"",
"./Optimized_Implementation/avx2/III_pkc:":	"",
"./Optimized_Implementation/avx2/III_pkc_skc:":	"",
"./Optimized_Implementation/avx2/Ip:":	"",
"./Optimized_Implementation/avx2/Ip_pkc:":	"",
"./Optimized_Implementation/avx2/Ip_pkc_skc:":	"",
"./Optimized_Implementation/avx2/Is:":	"",
"./Optimized_Implementation/avx2/Is_pkc:":	"",
"./Optimized_Implementation/avx2/Is_pkc_skc:":	"",
"./Optimized_Implementation/avx2/V:":	"",
"./Optimized_Implementation/avx2/V_pkc:":	"",
"./Optimized_Implementation/avx2/V_pkc_skc:":	"",
"./Optimized_Implementation/avx2/nistkat:":	"",
"./Optimized_Implementation/avx2:":	"",
"./Optimized_Implementation/neon/III:":	"",
"./Optimized_Implementation/neon/III_pkc:":	"",
"./Optimized_Implementation/neon/III_pkc_skc:":	"",
"./Optimized_Implementation/neon/Ip:":	"",
"./Optimized_Implementation/neon/Ip_pkc:":	"",
"./Optimized_Implementation/neon/Ip_pkc_skc:":	"",
"./Optimized_Implementation/neon/Is:":	"",
"./Optimized_Implementation/neon/Is_pkc:":	"",
"./Optimized_Implementation/neon/Is_pkc_skc:":	"",
"./Optimized_Implementation/neon/V:":	"",
"./Optimized_Implementation/neon/V_pkc:":	"",
"./Optimized_Implementation/neon/V_pkc_skc:":	"",
"./Optimized_Implementation/neon/nistkat:":	"",
"./Optimized_Implementation/neon:":	"",
"./Optimized_Implementation:":	"",
"./Reference_Implementation/III:":	"",
"./Reference_Implementation/III_pkc:":	"",
"./Reference_Implementation/III_pkc_skc:":	"",
"./Reference_Implementation/Ip:":	"",
"./Reference_Implementation/Ip_pkc:":	"",
"./Reference_Implementation/Ip_pkc_skc:":	"",
"./Reference_Implementation/Is:":	"",
"./Reference_Implementation/Is_pkc:":	"",
"./Reference_Implementation/Is_pkc_skc:":	"",
"./Reference_Implementation/V:":	"",
"./Reference_Implementation/V_pkc:":	"",
"./Reference_Implementation/V_pkc_skc:":	"",
"./Reference_Implementation/nistkat:":	"",
"./Reference_Implementation:":	"",
".:":	"",
"III":	"",
"III_pkc":	"",
"III_pkc_skc":	"",
"Ip":	"",
"Ip_pkc":	"",
"Ip_pkc_skc":	"",
"Is":	"",
"Is_pkc":	"",
"Is_pkc_skc":	"",
"Makefile":	"",
"Optimized_Implementation":	"",
"PQCgenKAT_sign.c":             "the program for generating KATs from NIST",
"PQCsignKAT_1044336.req":       "KAT for requests",
"PQCsignKAT_1044336.rsp":       "KAT with responses",
"PQCsignKAT_237912.req":        "KAT for requests",
"PQCsignKAT_237912.rsp":        "KAT with responses",
"PQCsignKAT_2436720.req":       "KAT for requests",
"PQCsignKAT_2436720.rsp":       "KAT with responses",
"PQCsignKAT_348720.req":        "KAT for requests",
"PQCsignKAT_348720.rsp":        "KAT with responses",
"PQCsignKAT_48.req":            "KAT for requests",
"PQCsignKAT_48.rsp":            "KAT with responses",
"README.md":	"",
"Reference_Implementation":	"",
"V":	"",
"V_pkc":	"",
"V_pkc_skc":	"",
"aes128_4r_ffs.c":                "aes128 ref implementation of ffs method",
"aes128_4r_ffs.h":                "aes128 ref implementation of ffs method",
"aes_neonaes.c":                  "aes128 implementation of armv8 aes instruction",
"aes_neonaes.h":                  "aes128 implementation of armv8 aes instruction",
"amd64":                          "AMD64 optimization",
"api.h":                          "Defining API functions",
"avx2":                           "AVX2 optimization",
"blas.h":                         "Basic linear algebra functions",
"blas_avx2.h":                    "Linear algebra functions, AVX2 implementations",
"blas_comm.h":                    "Common functions for linear algebra",
"blas_matrix.c":                  "Linear algebra functions for matrix operations",
"blas_matrix.h":                  "Linear algebra functions for matrix operations",
"blas_matrix_avx2.c":             "Linear algebra functions for matrix operations, AVX2 implementations",
"blas_matrix_avx2.h":             "Linear algebra functions for matrix operations, AVX2 implementations",
"blas_matrix_neon.c":             "Linear algebra functions for matrix operations, NEON implementations",
"blas_matrix_neon.h":             "Linear algebra functions for matrix operations, NEON implementations",
"blas_matrix_ref.c":              "Linear algebra functions for matrix operations, ref implementations",
"blas_matrix_ref.h":              "Linear algebra functions for matrix operations, ref implementations",
"blas_matrix_sse.c":              "Linear algebra functions for matrix operations, SSE implementations",
"blas_matrix_sse.h":              "Linear algebra functions for matrix operations, SSE implementations",
"blas_neon.h":                    "Linear algebra functions, NEON implementations",
"blas_sse.h":                     "Linear algebra functions, SSE implemnetations",
"blas_u32.h":                     "Linear algebra functions, u32 implementations",
"blas_u64.h":                     "Linear algebra functions, u64 implementations",
"config.h":                       "Macros for configuring runtime options",
"fips202.c":                      "ref implementation for SHAKE256",
"fips202.h":                      "ref implementation for SHAKE256",
"generate_KAT.sh":                "script for generating KAT",
"gf16.h":                         "finte field arithmetic functions",
"gf16_avx2.h":                    "finte field arithmetic functions, AVX2 version",
"gf16_neon.h":                    "finte field arithmetic functions, NEON version",
"gf16_sse.h":                     "finte field arithmetic functions, SSE version",
"gf16_tabs.c":                    "tables for finte field arithmetic functions",
"gf16_tabs.h":                    "tables for finte field arithmetic functions",
"gf16_tabs_neon.c":               "tables for finte field arithmetic functions, neon version",
"gf16_tabs_neon.h":               "tables for finte field arithmetic functions, neon version",
"gf16_u64.h":                     "finte field arithmetic functions, u64 version",
"gf256_tabs.c":              "tables finte field arithmetic functions",
"gf256_tabs.h":              "tables finte field arithmetic functions",
"neon":                      "NEON optimization",
"nistkat":                   "Directory for KAT tools from NIST",
"ov.c":                      "Main functions for signing and verifying",
"ov.h":                      "Main functions for signing and verifying",
"ov_blas.h":                 "Wrappers for blas functions",
"ov_keypair.c":              "Formats of key pairs and functions for generating key pairs.",
"ov_keypair.h":              "Formats of key pairs and functions for generating key pairs.",
"ov_keypair_computation.c":  "Functions for computations of keypairs.",
"ov_keypair_computation.h":  "Functions for computations of keypairs.",
"ov_publicmap.c":            "Implementations for evaluating public keys",
"parallel_matrix_op.c":      "the standard implementations for functions in parallel_matrix_op.h",
"parallel_matrix_op.h":      "Librarys for operations of batched matrixes.",
"params.h":                  "Macros for defining parameters",
"rng.c":                     "KAT tools from NIST",
"rng.h":                     "KAT tools from NIST",
"sign.c":                    "the implementations for functions in api.h",
"sign_api-test.c":           "Unit-tester for api functions",
"utils.c":                   "Some utilities or Input/Oupput functions",
"utils.h":                   "Some utilities or Input/Oupput functions",
"utils_hash.c":              "Wrappers for HASH functions",
"utils_hash.h":              "Wrappers for HASH functions",
"utils_malloc.h":            "Utilities for memory allocation functions",
"utils_prng.c":              "Wrappers for PRNG functions",
"utils_prng.h":              "Wrappers for PRNG functions",
"utils_randombytes.c":       "Wrappers for randombytes()",
"utils_randombytes.h":       "Wrappers for randombytes()",
"x86aesni.c":                "AES implementation for x86 AES-NI instruction set",
"x86aesni.h":                "AES implementation for x86 AES-NI instruction set"
}






if __name__ == '__main__' :
  myset = set()
  for line in sys.stdin:
    word_list = line.split()
    if( word_list ):
      ss = word_list[-1]
      if( ss.isnumeric() ): continue
      if( ss == "." ) : continue
      if( ss == ".." ) : continue
      myset.add( ss )
      if ss in file_descriptions :
        print( '{0: <45}'.format( ss ) + file_descriptions[ss] )
      else :
        print( ss )
    else :
      print("")

  ## dump list in a dictionary form
  #print("---")
  #ll = []
  #for ele in myset:
  #  ll.append(ele)
  #ll.sort()
  #for l in ll :
  #  print( '"' + l + '":\t"",' )
