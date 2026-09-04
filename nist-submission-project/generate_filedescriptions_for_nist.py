#!/usr/bin/python

import sys
import os, errno, shutil
from shutil import copyfile

#
# HOW TO USE:
#   python3 this_file <project_dir>   # write <project_dir>/README
#   ls -alR | python3 this_file       # just the listing, on stdout
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
"./Optimized_Implementation/gfni/III:":	"",
"./Optimized_Implementation/gfni/III_pkc:":	"",
"./Optimized_Implementation/gfni/III_pkc_skc:":	"",
"./Optimized_Implementation/gfni/Ip:":	"",
"./Optimized_Implementation/gfni/Ip_pkc:":	"",
"./Optimized_Implementation/gfni/Ip_pkc_skc:":	"",
"./Optimized_Implementation/gfni/Is:":	"",
"./Optimized_Implementation/gfni/Is_pkc:":	"",
"./Optimized_Implementation/gfni/Is_pkc_skc:":	"",
"./Optimized_Implementation/gfni/V:":	"",
"./Optimized_Implementation/gfni/V_pkc:":	"",
"./Optimized_Implementation/gfni/V_pkc_skc:":	"",
"./Optimized_Implementation/gfni/nistkat:":	"",
"./Optimized_Implementation/gfni:":	"",
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
"III":                      "uov-III, classic",
"III_pkc":                  "uov-III, compressed public key",
"III_pkc_skc":              "uov-III, compressed public and secret keys",
"Ip":                       "uov-Ip, classic",
"Ip_pkc":                   "uov-Ip, compressed public key",
"Ip_pkc_skc":               "uov-Ip, compressed public and secret keys",
"Is":                       "uov-Is, classic",
"Is_pkc":                   "uov-Is, compressed public key",
"Is_pkc_skc":               "uov-Is, compressed public and secret keys",
"Makefile":                 "build rules, selecting a parameter set with PROJ",
"Optimized_Implementation": "optimized implementations, one directory per architecture",
"PQCgenKAT_sign.c":             "the program for generating KATs from NIST",
"README":	"the file you are reading",
"README.md":                "parameters and build instructions for this implementation",
"Reference_Implementation": "portable C reference implementation",
"V":                        "uov-V, classic",
"V_pkc":                    "uov-V, compressed public key",
"V_pkc_skc":                "uov-V, compressed public and secret keys",
"aes128_4r_ffs.c":                "aes128 ref implementation of ffs method",
"aes128_4r_ffs.h":                "aes128 ref implementation of ffs method",
"aes_neonaes.c":                  "aes128 implementation of armv8 aes instruction",
"aes_neonaes.h":                  "aes128 implementation of armv8 aes instruction",
"amd64":                          "AMD64 optimization",
"api.h":                          "Defining API functions",
"avx2":                           "AVX2 optimization",
"gfni":                           "GFNI optimization",
"blas.h":                         "Basic linear algebra functions",
"blas_avx2.h":                    "Linear algebra functions, AVX2 implementations",
"blas_avx2_gfni.h":               "Linear algebra functions, GFNI implementations",
"blas_comm.h":                    "Common functions for linear algebra",
"blas_matrix.c":                  "Linear algebra functions for matrix operations",
"blas_matrix.h":                  "Linear algebra functions for matrix operations",
"blas_matrix_avx2.c":             "Linear algebra functions for matrix operations, AVX2 implementations",
"blas_matrix_avx2.h":             "Linear algebra functions for matrix operations, AVX2 implementations",
"blas_matrix_avx2_gfni.c":        "Linear algebra functions for matrix operations, GFNI implementations",
"blas_matrix_avx2_gfni.h":        "Linear algebra functions for matrix operations, GFNI implementations",
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
"gf16_gfni.h":                    "finte field arithmetic functions, GFNI version",
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



def describe( name ):
  if name in file_descriptions : return file_descriptions[name]
  if name.startswith("PQCsignKAT_") :
    if name.endswith(".req") : return "KAT for requests"
    if name.endswith(".rsp") : return "KAT with responses"
  return None


def emit( name ):
  d = describe( name )
  print( name if d is None else '{0: <45}'.format( name ) + d )


def tree_lines( root ):
  """The same sequence of names that "ls -alR" feeds this script from stdin."""
  for dirpath, dirnames, filenames in os.walk( root ) :
    dirnames.sort()
    rel = os.path.relpath( dirpath , root )
    yield ( "." if rel == "." else "./" + rel ) + ":"
    for n in sorted( dirnames + filenames , key = str.lower ) : yield n
    yield ""


def listing( root ):
  import io, contextlib
  buf = io.StringIO()
  with contextlib.redirect_stdout( buf ) :
    for name in tree_lines( root ) :
      if name == "" : print( "" )
      else : emit( name )
  return buf.getvalue()


def write_readme( root , out_path , preamble = None ):
  """Section 2.C.4 of the NIST call: a plain text README listing every file."""
  open( out_path , "w" ).close()          # so the README lists itself
  text = listing( root )
  with open( out_path , "w" ) as fp :
    if preamble is not None :
      fp.write( preamble.rstrip( "\n" ) + "\n\n\n## File descriptions\n\n" )
    fp.write( text )


def readme_for( root ):
  """Write <root>/README: this directory's README.md.nist, then the listing."""
  here = os.path.dirname( os.path.abspath( __file__ ) )
  preamble = open( os.path.join( here , 'README.md.nist' ) ).read()
  write_readme( root , os.path.join( root , 'README' ) , preamble )


if __name__ == '__main__' :
  if 2 == len(sys.argv) :
    readme_for( sys.argv[1] )
    print( "wrote " + os.path.join( sys.argv[1] , 'README' ) )
    sys.exit()

  myset = set()
  for line in sys.stdin:
    word_list = line.split()
    if( word_list ):
      ss = word_list[-1]
      if( ss.isnumeric() ): continue
      if( ss == "." ) : continue
      if( ss == ".." ) : continue
      myset.add( ss )
      emit( ss )
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
