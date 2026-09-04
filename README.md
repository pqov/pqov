  
# OV signature

This project contains the reference and optimized implementations of the oil and vinegar (OV) signature system.

## Licence

Public Domain (https://creativecommons.org/share-your-work/public-domain/cc0/);
or Apache 2.0 License (https://www.apache.org/licenses/LICENSE-2.0.html)
unless stated differently at the top of each file. 

## Parameters

|         |NIST Sec. Level | Parameter (n,m,GF(q)) | epk size (bytes) | esk size (bytes) | cpk size (bytes)   | csk size (bytes)   | signature size (bytes) |
|:--------|:--------:|:-------------:|----------:|---------:|---------:|----------|----------:|
| uov-Ip  |1         |119,45,GF(256) | 321,300   |278,087   |46,591     | 32      |  135      |
| uov-Is  |1         |160,64,GF(16)  | 412,160   |348,704   |66,576     | 32      |   96      |
| uov-III |3         |193,72,GF(256) | 1,347,912 |1,167,440 |189,232    | 32      |  209      |
| uov-V   |5         |259,96,GF(256) | 3,232,320 |2,801,024 |446,992    | 32      |  275      |

## Contents

- **src** : Source code.
- **utils**  : utilities for AES, SHAKE, and PRNGs. The default setting calls openssl library.
- **unit_tests**  : unit testers.

## Generate NIST and SUPERCOP projects

1. NIST submission:  
  * creat basic nist project.
```
python3 ./create_nist_project.py
```
  * generate KAT files. (I personally recommend skipping this step for version controls of the source code because the size of KATs are enomerous.)
```
cd pqov_nist_submission/Optimized_Implementation/avx2/
source ./generate_KAT.sh
mv KAT ../../
cd ../../../
```
  * refresh the file descriptions required from NIST.  create_nist_project.py
    already wrote README, but the KAT files are not in it yet.
```
python3 ./nist-submission-project/generate_filedescriptions_for_nist.py pqov_nist_submission
```

2. SUPERCOP:  
```
python3 ./create_supercop_project.py
```


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

For compiling different parameters, we use the macros ( **_OV256_119_45** / **_OV256_193_72** / **_OV256_259_96** / **_OV16_160_64** ) to control the C source code.  
The default setting is **_OV256_119_45** defined in **src/params.h**.  

The other option is to use our makefile:  
1. **_OV16_160_64** :
```
make PARAM=1
```
2. **_OV256_119_45** :
```
make
```
or
```
make PARAM=3
```
3. **_OV256_193_72** :
```
make PARAM=4
```
4. **_OV256_259_96** :
```
make PARAM=5
```


### **Options for Variants:**

For compiling different variants, we use the macros ( **_OV_CLASSIC** / **_OV_PKC** / **_OV_PKC_SKC** ) to control the C source code.  
We use 4-rounds AES (macro: **\_4ROUND_AES_** ) as our alternative PRNG functions.  
The default setting is **_OV_CLASSIC** and _NO_ **\_4ROUND_AES_** defined in **src/params.h**.  

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
4. **_OV_CLASSIC** and **\_4ROUND_AES_** :
```
make VARIANT=4
```
5. **_OV_PKC** and **\_4ROUND_AES_** :
```
make VARIANT=5
```
6. **_OV_PKC_SKC** and **\_4ROUND_AES_** :
```
make VARIANT=6
```
7. **_OV256_259_96** and **_OV_PKC** :
```
make VARIANT=2 PARAM=5
```
8. **_OV256_259_96** and **_OV_PKC_SKC** and **\_4ROUND_AES_**:
```
make VARIANT=6 PARAM=5
```



### **Optimizations for Architectures:**

#### **Reference Version:**
The reference uses (1) source code in the directories: **src/** , **src/ref/**, and  
(2) directories for utilities of AES, SHAKE, and randombytes() : **utils/** .  
The default implementation for AES and SHAKE is from openssl library, controlled by the macro **_UTILS_OPENSSL\_** defined in **src/config.h**.  

Or, use our makefile:  
1. Reference version (**_OV256_119_45** and **_OV_CLASSIC**):
```
make
```
2. Reference version, **_OV256_259_96** , and **_OV_PKC** :
```
make VARIANT=2 PARAM=5
```


To turn on the option of 4-round AES, one need to turn on the macro **_4ROUND_AES\_** defined in **src/params.h**.  


#### **AVX2 Version:**
The AVX2 option uses (1) source code in the directories: **src/** , **src/amd64** , **src/ssse3** , **src/avx2**, and  
(2) directories for utilities of AES, SHAKE, and randombytes() : **utils/**, **utils/x86aesni** .  
(3) One stil need to turn on the macros **_BLAS_AVX2\_**, **_MUL_WITH_MULTAB\_**, **_UTILS_AESNI\_** defined in **src/config.h** to enable AVX2 optimization.  

Or, use our makefile:  
1. AVX2 version (**_OV256_119_45** and **_OV_CLASSIC**):
```
make PROJ=avx2
```
2. AVX2 version, **_OV256_193_72**, and **_OV_PKC**:
```
make PROJ=avx2 PARAM=4 VARIANT=2
```

#### **NEON Version:**
The NEON option uses (1) source code in the **src/** , **src/amd64** , **src/neon**, and  
(2) directories for utilities of AES, SHAKE, and randombytes() : **utils/**, ( **utils/neon_aesinst** (Armv8 AES instruction) or **utils/neon_aes**(NEON bitslice AES implemetation) ).  
(3) One stil need to turn on the macros **_BLAS_NEON\_** , **_UTILS_NEONAES\_** defined in **src/config.h** to enable NEON optimization.  
(4) Depending on the CPUs and parameters, one can choose to define the macro **_MUL_WITH_MULTAB\_** for GF multiplication with MUL tables. We suggest to turn on it for the **_OV16_160_64** parameter.  

Or, use our makefile:  
1. NEON version (**_OV256_119_45** and **_OV_CLASSIC**):
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
to detect if running on Mac OS and 
```
uname -m
```
to detect if is an Arm-based Mac.
If `uname -s` returns **Darwin** and `uname -m` returns **arm64, we are running
on an Arm-based Mac (e.g., Apple M1).
The Makefile will then define the **_APPLE_SILICON\_** macro for enabling some optimization settings in the source code .  
2. The program needs **sudo** to benchmark on Mac OS correctly.

## Benchmarks

Cycle counts from **rec-sign-benchmark**, with turbo boost and SMT off, and
with address-space randomization disabled for the measured process.
To reproduce a row, build and run the configuration directly:

```
make clean && make rec-sign-benchmark PROJ=avx2 PARAM=3 VARIANT=1
setarch $(uname -m) -R taskset -c 0 ./rec-sign-benchmark
```

`PARAM` is 1, 3, 4, 5 for OV(16,160,64), OV(256,119,45), OV(256,193,72),
OV(256,259,96); `VARIANT` is 1, 2, 3 for classic, pkc and pkc+skc.  The tables
below report the median of each triple.  On macOS the program needs **sudo**
to read the cycle counter.

### Intel(R) Xeon(R) CPU E3-1230L v3 @ 1.80GHz, avx2
gcc-15 -- gcc-15 (Ubuntu 15.2.0-14ubuntu1~24~ppa1) 15.2.0, measured at f3e7cd9, 2026-08-24
median of 1000 signatures and 100 key generations

| Parameter | key generation | signing | sign-opening |
|:----------|---------------:|--------:|-------------:|
| OV(16,160,64)-classic | 4,895,116 | 139,616 | 66,408 |
| OV(16,160,64)-pkc | 4,846,220 | 136,712 | 409,428 |
| OV(16,160,64)-pkc-skc | 4,362,000 | 678,528 | 409,532 |
| OV(256,119,45)-classic | 3,587,232 | 120,984 | 83,724 |
| OV(256,119,45)-pkc | 3,537,640 | 121,636 | 354,036 |
| OV(256,119,45)-pkc-skc | 3,523,032 | 573,328 | 354,560 |
| OV(256,193,72)-classic | 22,512,648 | 327,644 | 245,868 |
| OV(256,193,72)-pkc | 22,337,004 | 326,116 | 1,383,836 |
| OV(256,193,72)-pkc-skc | 23,996,620 | 2,111,064 | 1,383,436 |
| OV(256,259,96)-classic | 66,626,076 | 673,972 | 463,424 |
| OV(256,259,96)-pkc | 65,580,064 | 696,540 | 3,183,152 |
| OV(256,259,96)-pkc-skc | 59,292,764 | 4,499,944 | 3,188,812 |

### Intel(R) Xeon(R) CPU E3-1275 v5 @ 3.60GHz, avx2
gcc-15 -- gcc-15 (Ubuntu 15.2.0-14ubuntu1~24~ppa1) 15.2.0, measured at f3e7cd9, 2026-08-24
median of 1000 signatures and 100 key generations

| Parameter | key generation | signing | sign-opening |
|:----------|---------------:|--------:|-------------:|
| OV(16,160,64)-classic | 4,253,780 | 120,526 | 60,660 |
| OV(16,160,64)-pkc | 4,314,950 | 123,104 | 286,888 |
| OV(16,160,64)-pkc-skc | 3,978,980 | 520,234 | 287,248 |
| OV(256,119,45)-classic | 3,275,886 | 117,808 | 74,252 |
| OV(256,119,45)-pkc | 3,189,084 | 117,422 | 253,122 |
| OV(256,119,45)-pkc-skc | 3,218,816 | 471,258 | 252,876 |
| OV(256,193,72)-classic | 18,459,512 | 299,814 | 216,772 |
| OV(256,193,72)-pkc | 18,360,852 | 300,064 | 968,974 |
| OV(256,193,72)-pkc-skc | 18,851,832 | 1,628,832 | 967,100 |
| OV(256,259,96)-classic | 53,227,288 | 648,574 | 433,102 |
| OV(256,259,96)-pkc | 51,692,258 | 662,056 | 2,229,086 |
| OV(256,259,96)-pkc-skc | 48,217,758 | 3,467,808 | 2,276,388 |

### AMD EPYC 9754 128-Core Processor @ 2.25 GHz, avx2
gcc-15 -- gcc-15 (Ubuntu 15.2.0-14ubuntu1~24~ppa1) 15.2.0, measured at f3e7cd9, 2026-08-24
median of 1000 signatures and 100 key generations

| Parameter | key generation | signing | sign-opening |
|:----------|---------------:|--------:|-------------:|
| OV(16,160,64)-classic | 2,778,030 | 75,262 | 46,373 |
| OV(16,160,64)-pkc | 2,739,263 | 76,163 | 178,627 |
| OV(16,160,64)-pkc-skc | 2,593,552 | 353,970 | 179,280 |
| OV(256,119,45)-classic | 2,250,945 | 80,663 | 59,265 |
| OV(256,119,45)-pkc | 2,228,602 | 80,550 | 163,620 |
| OV(256,119,45)-pkc-skc | 2,230,920 | 351,990 | 164,430 |
| OV(256,193,72)-classic | 12,285,810 | 220,973 | 170,145 |
| OV(256,193,72)-pkc | 12,215,610 | 222,773 | 614,633 |
| OV(256,193,72)-pkc-skc | 12,561,592 | 1,156,117 | 618,255 |
| OV(256,259,96)-classic | 34,319,002 | 428,220 | 307,575 |
| OV(256,259,96)-pkc | 34,088,242 | 433,193 | 1,374,008 |
| OV(256,259,96)-pkc-skc | 33,715,462 | 2,242,058 | 1,376,258 |

### AMD EPYC 9754 128-Core Processor @ 2.25 GHz, avx2gfni
gcc-15 -- gcc-15 (Ubuntu 15.2.0-14ubuntu1~24~ppa1) 15.2.0, measured at f3e7cd9, 2026-08-24
median of 1000 signatures and 100 key generations

| Parameter | key generation | signing | sign-opening |
|:----------|---------------:|--------:|-------------:|
| OV(16,160,64)-classic | 1,897,403 | 64,575 | 46,058 |
| OV(16,160,64)-pkc | 1,859,445 | 65,183 | 178,132 |
| OV(16,160,64)-pkc-skc | 1,646,955 | 313,493 | 178,448 |
| OV(256,119,45)-classic | 1,293,255 | 51,930 | 59,040 |
| OV(256,119,45)-pkc | 1,275,570 | 52,695 | 163,260 |
| OV(256,119,45)-pkc-skc | 1,275,300 | 318,690 | 163,867 |
| OV(256,193,72)-classic | 6,261,053 | 144,720 | 171,652 |
| OV(256,193,72)-pkc | 6,154,605 | 148,545 | 616,410 |
| OV(256,193,72)-pkc-skc | 6,968,227 | 1,028,160 | 617,625 |
| OV(256,259,96)-classic | 19,811,498 | 294,660 | 305,617 |
| OV(256,259,96)-pkc | 19,591,898 | 306,518 | 1,370,700 |
| OV(256,259,96)-pkc-skc | 15,494,220 | 1,767,915 | 1,374,525 |

### Apple M1 @ 3.2 GHz, neon
cc -- Apple clang version 21.0.0 (clang-2100.1.1.101), measured at cc68b0d, 2026-08-15
median of 1000 signatures and 100 key generations

| Parameter | key generation | signing | sign-opening |
|:----------|---------------:|--------:|-------------:|
| OV(16,160,64)-classic | 3,365,033 | 92,169 | 47,119 |
| OV(16,160,64)-pkc | 3,354,034 | 92,027 | 138,893 |
| OV(16,160,64)-pkc-skc | 3,335,698 | 328,589 | 139,679 |
| OV(256,119,45)-classic | 1,968,806 | 65,322 | 56,343 |
| OV(256,119,45)-pkc | 1,949,388 | 65,266 | 129,550 |
| OV(256,119,45)-pkc-skc | 1,949,955 | 276,914 | 132,062 |
| OV(256,193,72)-classic | 10,775,617 | 195,764 | 204,020 |
| OV(256,193,72)-pkc | 10,692,548 | 195,640 | 508,693 |
| OV(256,193,72)-pkc-skc | 10,693,114 | 804,029 | 507,430 |
| OV(256,259,96)-classic | 33,342,751 | 413,876 | 420,461 |
| OV(256,259,96)-pkc | 31,216,115 | 412,978 | 1,148,529 |
| OV(256,259,96)-pkc-skc | 31,223,355 | 1,743,448 | 1,148,180 |

### Cortex-A72 (Raspberry Pi 4 Model B Rev 1.4) @ 1.80 GHz, neon
cc -- cc (Debian 12.2.0-14) 12.2.0, measured at d4549d3, 2026-08-21
median of 1000 signatures and 100 key generations

| Parameter | key generation | signing | sign-opening |
|:----------|---------------:|--------:|-------------:|
| OV(16,160,64)-classic | 28,733,459 | 572,985 | 153,178 |
| OV(16,160,64)-pkc | 28,423,107 | 651,190 | 5,700,519 |
| OV(16,160,64)-pkc-skc | 28,290,786 | 6,828,398 | 5,735,516 |
| OV(256,119,45)-classic | 13,406,322 | 311,347 | 172,267 |
| OV(256,119,45)-pkc | 13,299,887 | 341,984 | 4,823,957 |
| OV(256,119,45)-pkc-skc | 13,288,337 | 5,288,893 | 4,846,927 |
| OV(256,193,72)-classic | 75,561,941 | 1,621,046 | 624,398 |
| OV(256,193,72)-pkc | 82,837,385 | 1,709,393 | 21,464,891 |
| OV(256,193,72)-pkc-skc | 78,839,423 | 22,527,376 | 21,383,206 |
| OV(256,259,96)-classic | 354,823,805 | 3,616,073 | 1,467,984 |
| OV(256,259,96)-pkc | 355,522,310 | 3,659,781 | 49,796,516 |
| OV(256,259,96)-pkc-skc | 350,657,976 | 54,891,988 | 49,801,859 |
