This repository contains the code accompanying the paper **Oil and Vinegar: Modern Parameters and
Implementations** which is available [here](https://eprint.iacr.org/2023/059).

This repository contains OV implementations targeting x86 (with AVX2), Armv8 (with Neon), Arm Cortex-M4, and FPGA. 

Authors: 
 - Ward Beullens
 - Ming-Shing Chen
 - Shih-Hao Hung
 - Matthias J. Kannwischer
 - Bo-Yuan Peng
 - Cheng-Jhih Shih
 - Bo-Yin Yang
 
 
**Warning: This is the version of the code accompanying the paper. This is not the NIST submission! Parameters and implementations may still change for the NIST submission. Official reference code will be posted separately.**

# Parameters 

| Parameter    | signature size | pk size  | sk size | pkc size | compressed-sk size |  
|:-----------: |:--------------:|--------- |---------|------------|--------------------|
|GF(16),160,64 | 96             |412,160   |348,720  |66,576      | 48                 |
|GF(256),112,44| 128            |278,432   |237,912  |43,576      | 48                 |
|GF(256),184,72| 200            |1,225,440 |1,044,336|189,232     | 48                 |
|GF(256),244,96| 260            |2,869,440 |2,436,720|446,992     | 48                 |

# Cortex-M4 
For the Cortex-M4 implementations, see [m4/README.md](./m4/README.md)

# FPGA
For the FPGA implementations, see [fpga/README.md](./fpga/README.md)

# x86 (AVX2) and Armv8 (Neon)

## Contents

- **src** : Source code.
- **utils**  : utilities for AES, SHAKE, and PRNGs. The default setting calls openssl library.
- **unit_tests**  : unit testers.

## Instructions for testing/benchmarks

Type **make**    
```
make
```
for generating 3 executables:  
1. sign_api-test : testing for API functions (crypto_keygen(), crypto_sign(), and crypto_verify()).  
2. sign_api-benchmark: reporting performance numbers for signature API functions.  
2. rec-sign-benchmark: reporting more detailed performance numbers for signature API functions. Number format: ''average /numbers of testing (1st quartile, median, 3rd quartile)''  


### **Options for Parameters:**

For compiling different parameters, we use the macros ( **_OV256_112_44** / **_OV256_184_72** / **_OV256_244_96** / **_OV16_160_64** ) to control the C source code.  
The default setting is **_OV256_112_44** defined in **src/params.h**.  

The other option is to use our makefile:  
1. **_OV16_160_64** :
```
make PARAM=1
```
2. **_OV256_112_44** :
```
make
```
or
```
make PARAM=3
```
3. **_OV256_184_72** :
```
make PARAM=4
```
4. **_OV256_244_96** :
```
make PARAM=5
```


### **Options for Variants:**

For compiling different variants, we use the macros ( **_OV_CLASSIC** / **_OV_PKC** / **_OV_PKC_SKC** ) to control the C source code.  
The default setting is **_OV_CLASSIC** defined in **src/params.h**.  

The other option is to use our makefile:  
1. **_OV_CLASSIC** :
```
make
```
or
```
make VARIANT=1
```
2. **_OV_PKC** :
```
make VARIANT=2
```
3. **_OV_PKC_SKC** :
```
make VARIANT=3
```
4. **_OV256_244_96** and **_OV_PKC** :
```
make VARIANT=2 PARAM=5
```

### **Optimizations for Architectures:**

#### **Reference Version:**
The reference uses (1) source code in the directories: **src/** , **src/ref/**, and  
(2) directories for utilities of AES, SHAKE, and randombytes() : **utils/** .  
The default implementation for AES and SHAKE is from openssl library, controlled by the macro **_UTILS_OPENSSL_** defined in **src/config.h**.  

Or, use our makefile:  
1. Reference version (**_OV256_112_44** and **_OV_CLASSIC**):
```
make
```
2. Reference version, **_OV256_244_96** , and **_OV_PKC** :
```
make VARIANT=2 PARAM=5
```


To turn on the option of 4-round AES, one need to turn on the macro **_4ROUND_AES\_** defined in **src/params.h**.  


#### **AVX2 Version:**
The AVX2 option uses (1) source code in the directories: **src/** , **src/amd64** , **src/ssse3** , **src/avx2**, and  
(2) directories for utilities of AES, SHAKE, and randombytes() : **utils/**, **utils/x86aesni** .  
(3) One stil need to turn on the macros **_BLAS_AVX2\_**, **_MUL_WITH_MULTAB\_**, **_UTILS_AESNI\_** defined in **src/config.h** to enable AVX2 optimization.  

Or, use our makefile:  
1. AVX2 version (**_OV256_112_44** and **_OV_CLASSIC**):
```
make PROJ=avx2
```
2. AVX2 version, **_OV256_184_72**, and **_OV_PKC**:
```
make PROJ=avx2 PARAM=4 VARIANT=2
```

#### **NEON Version:**
The NEON option uses (1) source code in the **src/** , **src/amd64** , **src/neon**, and  
(2) directories for utilities of AES, SHAKE, and randombytes() : **utils/**, ( **utils/neon_aesinst** (Armv8 AES instruction) or **utils/neon_aes**(NEON bitslice AES implemetation) ).  
(3) One stil need to turn on the macros **_BLAS_NEON\_** , **_UTILS_NEONAES\_** defined in **src/config.h** to enable NEON optimization.  
(4) Depending on the CPUs and parameters, one can choose to define the macro **_MUL_WITH_MULTAB\_** for GF multiplication with MUL tables. We suggest to turn on it for the **_OV16_160_64** parameter.  

Or, use our makefile:  
1. NEON version (**_OV256_112_44** and **_OV_CLASSIC**):
```
make PROJ=neon
```
2. Another example: NEON version, **_OV16_160_64**, and **_OV_PKC_SKC**:
```
make PROJ=neon PARAM=1 VARIANT=3
```

Notes for **Apple Mac M1**:  
1. We use
```
uname -s
```
to detect if running on Mac OS.
If **uname** returns string containing **Darwin**,
the makefile will define **_MAC_OS\_** macro for enabling some optimization settings in the source code .  
2. The program needs **sudo** to benchmark on Mac OS correctly.


### **Options for Algorithm of Solving Linear Equation while Signing:**
1. Default setting: Gaussian Elimination and backward substitution.
2. Choose the algorithm of calculating inversion matrix with block matrix compution:  
  (a) Define the **_LDU_DECOMPOSE\_** macro in **src/parms.h**.  
  (b) Remove the **_BACK_SUBSTITUTION\_** macro in **src/ov.c**.  


# License

Our implementations of OV are released under the conditions of [CC0](https://creativecommons.org/publicdomain/zero/1.0/).
Third party code may have other licenses which is stated at the top of each file or in the respective LICENSE files.
