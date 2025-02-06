  
# OV signature

This project contains the reference and optimized implementations of the oil and vinegar (OV) signature system.

## Licence

Public domain unless stated differently at the top of each file.

## Parameters

| Parameter    | signature size | pk size  | sk size | pkc size | compressed-sk size |  
|:-----------: |:--------------:|--------- |---------|------------|--------------------|
|GF(16),160,64 | 96             |412,160   |348,704  |66,576      | 32                 |
|GF(256),112,44| 128            |278,432   |237,896  |43,576      | 32                 |
|GF(256),184,72| 200            |1,225,440 |1,044,320|189,232     | 32                 |
|GF(256),244,96| 260            |2,869,440 |2,436,704|446,992     | 32                 |

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
  * generate file descriptions required from NIST.
```
cd pqov_nist_submission/
ls -alR | python ../generate_filedescriptions_for_nist.py >> README
cd ../
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
7. **_OV256_244_96** and **_OV_PKC** :
```
make VARIANT=2 PARAM=5
```
8. **_OV256_244_96** and **_OV_PKC_SKC** and **\_4ROUND_AES_**:
```
make VARIANT=6 PARAM=5
```



### **Optimizations for Architectures:**

#### **Reference Version:**
The reference uses (1) source code in the directories: **src/** , **src/ref/**, and  
(2) directories for utilities of AES, SHAKE, and randombytes() : **utils/** .  
The default implementation for AES and SHAKE is from openssl library, controlled by the macro **_UTILS_OPENSSL\_** defined in **src/config.h**.  

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

1. AVX2 implementations on Intel(R) Xeon(R) CPU E3-1275 v5 @ 3.60GHz (Skylake), turbo boost off

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(256,112,44)-classic |  2892523.100000 /100 (2804736,2815370,2831920) |  108127.316000 /1000 (106738,106896,107090) |  80951.408000 /1000 (80408,80752,81092) |
| OV(256,112,44)-pkc |  2824726.380000 /100 (2748124,2751978,2754982) |  110427.668000 /1000 (109536,109678,109838) |  239040.854000 /1000 (237528,237870,238264) |
| OV(256,112,44)-pkc-skc |  2844974.980000 /100 (2719804,2721876,2723084) |  1793493.076000 /1000 (1785630,1793232,1797256) |  236290.334000 /1000 (235110,235512,235988) |
| OV(16,160,64)-classic |  4801021.320000 /100 (4713236,4716210,4723996) |  132300.866000 /1000 (124028,124552,124874) |  60779.184000 /1000 (59862,60644,61426) |
| OV(16,160,64)-pkc |  4934570.880000 /100 (4807262,4811688,4820428) |  135517.336000 /1000 (126756,127310,127752) |  283991.938000 /1000 (282092,282786,283564) |
| OV(16,160,64)-pkc-skc |  4992098.920000 /100 (4910810,4915100,4918890) |  2846741.604000 /1000 (2845170,2857010,2858378) |  284371.878000 /1000 (282828,283652,284416) |
| OV(256,184,72)-classic |  17027680.540000 /100 (16903554,16920546,16930966) |  297492.796000 /1000 (295532,295706,295938) |  237227.274000 /1000 (236038,236524,237088) |
| OV(256,184,72)-pkc |  16987281.820000 /100 (16781132,16804762,16821644) |  298224.890000 /1000 (295760,296160,296536) |  961911.762000 /1000 (954454,957878,964724) |
| OV(256,184,72)-pkc-skc |  15929406.760000 /100 (15784246,15801494,15810482) |  9779247.586000 /1000 (9698244,9801154,9806816) |  956957.072000 /1000 (953094,954018,963856) |
| OV(256,244,96)-classic |  45326890.200000 /100 (45248920,45267112,45286314) |  574518.202000 /1000 (570226,571746,573778) |  452817.836000 /1000 (449828,451156,452990) |
| OV(256,244,96)-pkc |  45287053.240000 /100 (45210934,45220438,45231298) |  587925.448000 /1000 (581318,584642,588060) |  2059494.200000 /1000 (2046612,2056902,2059902) |
| OV(256,244,96)-pkc-skc |  41880245.560000 /100 (41752732,41760798,41766912) |  22046811.838000 /1000 (22066984,22070084,22073672) |  2047746.986000 /1000 (2041114,2050878,2053028) |

2. With _4RAES_ macro, AVX2 implementations on Intel(R) Xeon(R) CPU E3-1275 v5 @ 3.60GHz (Skylake), turbo boost off

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(256,112,44)-classic |  2846597.820000 /100 (2713398,2718248,2723784) |  107836.088000 /1000 (106520,106700,106880) |  80827.668000 /1000 (80180,80584,80962) |
| OV(256,112,44)-pkc |  2761500.360000 /100 (2673318,2679040,2683126) |  107616.878000 /1000 (106920,107098,107276) |  149904.068000 /1000 (148392,148790,149294) |
| OV(256,112,44)-pkc-skc |  2710694.760000 /100 (2618780,2621688,2632382) |  1698783.052000 /1000 (1692514,1695062,1704830) |  147121.936000 /1000 (146238,146604,146976) |
| OV(16,160,64)-classic |  4999648.100000 /100 (4913456,4916810,4925414) |  141074.030000 /1000 (131028,131686,132284) |  61794.136000 /1000 (60880,61688,62480) |
| OV(16,160,64)-pkc |  4708477.000000 /100 (4616922,4623460,4649108) |  133391.380000 /1000 (125206,125632,126322) |  155261.920000 /1000 (153654,154362,155040) |
| OV(16,160,64)-pkc-skc |  4659529.440000 /100 (4577696,4579952,4588058) |  2717790.004000 /1000 (2711142,2721914,2723310) |  155047.138000 /1000 (153498,154184,154926) |
| OV(256,184,72)-classic |  16708274.000000 /100 (16529422,16539208,16559228) |  298451.438000 /1000 (295664,295858,296080) |  274891.498000 /1000 (273378,274034,274618) |
| OV(256,184,72)-pkc |  16510618.700000 /100 (16201182,16433866,16449156) |  295060.906000 /1000 (293150,293390,293870) |  569995.878000 /1000 (565364,566172,567366) |
| OV(256,184,72)-pkc-skc |  15571710.540000 /100 (15435118,15443444,15464332) |  9283987.446000 /1000 (9275502,9279918,9284966) |  531990.932000 /1000 (529774,530526,531394) |
| OV(256,244,96)-classic |  44742964.400000 /100 (44560052,44568542,44582140) |  576853.320000 /1000 (570806,571744,573236) |  438109.496000 /1000 (435096,436832,438858) |
| OV(256,244,96)-pkc |  44267741.720000 /100 (44203250,44294228,44302364) |  589625.072000 /1000 (583838,585434,588058) |  1089329.452000 /1000 (1078460,1081122,1091084) |
| OV(256,244,96)-pkc-skc |  40890868.480000 /100 (40656058,40661334,40670324) |  21121179.054000 /1000 (21169942,21181926,21188142) |  1074500.850000 /1000 (1070032,1072002,1080576) |


3. AVX2 implementations on Intel(R) Xeon(R) CPU E3-1230L v3 @ 1.80GHz (Haswell)), turbo boost off

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(256,112,44)-classic |  3299299.840000 /100 (3230040,3232108,3233624) |  113550.960000 /1000 (110608,111004,115540) |  87882.616000 /1000 (86224,86600,89264) |
| OV(256,112,44)-pkc |  3175055.080000 /100 (3087140,3090240,3095028) |  112505.480000 /1000 (111232,111472,111760) |  338762.720000 /1000 (331024,334116,341608) |
| OV(256,112,44)-pkc-skc |  3339452.360000 /100 (3266000,3272068,3276408) |  2344969.068000 /1000 (2337300,2341136,2348892) |  326928.736000 /1000 (315312,318108,334248) |
| OV(16,160,64)-classic |  5541228.040000 /100 (5438480,5443696,5521692) |  140375.360000 /1000 (131748,131968,132244) |  65814.316000 /1000 (64708,65460,66268) |
| OV(16,160,64)-pkc |  5413457.160000 /100 (5327268,5329940,5336024) |  143333.304000 /1000 (132180,132416,132724) |  418544.124000 /1000 (409912,413556,418916) |
| OV(16,160,64)-pkc-skc |  5550142.600000 /100 (5457912,5462040,5465060) |  3467653.196000 /1000 (3451140,3454352,3475576) |  443630.096000 /1000 (432980,438104,445680) |
| OV(256,184,72)-classic |  20471534.720000 /100 (20360872,20367780,20381148) |  337061.288000 /1000 (332748,333804,334680) |  340705.756000 /1000 (337512,338732,340740) |
| OV(256,184,72)-pkc |  20427995.480000 /100 (20321236,20340504,20368556) |  336819.300000 /1000 (333396,333748,334380) |  1414175.264000 /1000 (1374028,1384476,1413912) |
| OV(256,184,72)-pkc-skc |  19554206.440000 /100 (19019380,19029344,20542516) |  10936258.820000 /1000 (10892056,10913044,10972400) |  1396571.112000 /1000 (1361968,1372056,1386924) |
| OV(256,244,96)-classic |  55853499.320000 /100 (54868384,54885660,54932200) |  607370.464000 /1000 (600972,601872,613920) |  485913.828000 /1000 (482216,482840,493496) |
| OV(256,244,96)-pkc |  53757848.520000 /100 (53642040,53650248,53657708) |  615410.148000 /1000 (607148,608984,620660) |  2994051.436000 /1000 (2947108,2963524,2988852) |
| OV(256,244,96)-pkc-skc |  49072560.400000 /100 (48869540,48880100,48894100) |  25799829.944000 /1000 (25722372,25762436,25845100) |  2989307.660000 /1000 (2935080,2957824,3000564) |

4. Wit _4RAES_ macro, AVX2 implementations on Intel(R) Xeon(R) CPU E3-1230L v3 @ 1.80GHz (Haswell)), turbo boost off

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(256,112,44)-classic |  3319470.320000 /100 (3249084,3250380,3251828) |  112659.108000 /1000 (112016,112184,112364) |  87094.296000 /1000 (85852,86104,86408) |
| OV(256,112,44)-pkc |  3139144.440000 /100 (3069384,3072204,3075572) |  111485.248000 /1000 (110504,110736,111008) |  213930.056000 /1000 (211656,212664,214052) |
| OV(256,112,44)-pkc-skc |  3009180.760000 /100 (2935896,2942796,2944424) |  2049807.260000 /1000 (2048012,2048484,2049104) |  188280.088000 /1000 (186904,187184,187496) |
| OV(16,160,64)-classic |  5274541.120000 /100 (5199140,5201920,5204756) |  140093.816000 /1000 (131816,132012,132284) |  66087.164000 /1000 (64852,65584,66400) |
| OV(16,160,64)-pkc |  5498708.120000 /100 (5431608,5434316,5436628) |  140969.208000 /1000 (131700,131920,132208) |  212413.084000 /1000 (210504,211292,212140) |
| OV(16,160,64)-pkc-skc |  5401375.440000 /100 (5327680,5329984,5332504) |  3145354.736000 /1000 (3136652,3139844,3141728) |  214168.716000 /1000 (212220,213020,213840) |
| OV(256,184,72)-classic |  20479751.160000 /100 (20367508,20373544,20389828) |  336248.180000 /1000 (332704,333048,333560) |  353665.220000 /1000 (350872,352332,354500) |
| OV(256,184,72)-pkc |  19516394.040000 /100 (19423004,19427136,19440076) |  336994.484000 /1000 (333064,333416,333892) |  719228.144000 /1000 (713164,713956,723580) |
| OV(256,184,72)-pkc-skc |  19499468.200000 /100 (19379236,19385244,19393292) |  10368321.812000 /1000 (10356972,10360400,10371592) |  790628.344000 /1000 (785864,788332,795704) |
| OV(256,244,96)-classic |  54198217.360000 /100 (53636088,53652080,53676708) |  613536.748000 /1000 (603816,605044,617644) |  596678.380000 /1000 (591728,593924,603824) |
| OV(256,244,96)-pkc |  52891211.800000 /100 (52768256,52778696,52794972) |  639228.452000 /1000 (628932,631656,643196) |  1460105.820000 /1000 (1455536,1458028,1459692) |
| OV(256,244,96)-pkc-skc |  48149279.200000 /100 (47957916,47964024,47976388) |  24832271.824000 /1000 (24814960,24823412,24832048) |  1447772.384000 /1000 (1446708,1449052,1450184) |


5. neon implementation on Apple M1

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(256,112,44)-classic |  1668551.680000 /100 (1666460,1666693,1666978) |  60567.792000 /1000 (60361,60559,60745) |  49345.703000 /1000 (49096,49257,49499) |
| OV(256,112,44)-pkc |  1649390.280000 /100 (1646214,1646625,1647951) |  60942.065000 /1000 (60345,60551,60741) |  112718.061000 /1000 (112358,112557,112831) |
| OV(256,112,44)-pkc-skc |  2006902.560000 /100 (1653965,1658789,1796804) |  1063955.268000 /1000 (1060195,1060675,1067752) |  114090.243000 /1000 (113184,113434,113877) |
| OV(16,160,64)-classic |  3478392.360000 /100 (3351458,3353769,3357914) |  98375.553000 /1000 (92175,92367,92628) |  46423.170000 /1000 (45209,46366,47576) |
| OV(16,160,64)-pkc |  3332315.920000 /100 (3327520,3329499,3334999) |  98016.814000 /1000 (91947,92166,92412) |  138854.858000 /1000 (136579,138842,141183) |
| OV(16,160,64)-pkc-skc |  3352154.510000 /100 (3347101,3349631,3354143) |  2104673.377000 /1000 (2095295,2095931,2103887) |  139569.418000 /1000 (137326,139558,141779) |
| OV(256,184,72)-classic |  9576229.570000 /100 (9566280,9573288,9580363) |  186566.424000 /1000 (185274,185700,186148) |  184712.717000 /1000 (184356,184615,184916) |
| OV(256,184,72)-pkc |  9502006.270000 /100 (9497194,9500736,9502886) |  192211.036000 /1000 (190080,190540,191046) |  458979.041000 /1000 (458615,458891,459239) |
| OV(256,184,72)-pkc-skc |  9518484.000000 /100 (9514398,9515409,9519332) |  6257699.486000 /1000 (6251510,6259053,6259744) |  459518.283000 /1000 (458296,458693,459306) |
| OV(256,244,96)-classic |  28246451.690000 /100 (28219453,28228123,28243547) |  388967.671000 /1000 (380382,393047,395551) |  371960.531000 /1000 (371338,371702,372198) |
| OV(256,244,96)-pkc |  27634490.030000 /100 (26751886,28143066,28156421) |  398596.867000 /1000 (395068,395827,397742) |  1006930.869000 /1000 (1006085,1006448,1006925) |
| OV(256,244,96)-pkc-skc |  26923929.260000 /100 (26662077,26668781,26718313) |  16056432.885000 /1000 (16047195,16051661,16055396) |  1029977.949000 /1000 (1011654,1034885,1037780) |


6. neon implementation on Apple M1, defining _4RAES_ macro

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(256,112,44)-classic |  1638710.990000 /100 (1636917,1637123,1637301) |  60887.892000 /1000 (60433,60626,60788) |  49259.339000 /1000 (49008,49162,49416) |
| OV(256,112,44)-pkc |  1617998.490000 /100 (1616221,1616346,1616458) |  60938.352000 /1000 (60406,60606,60797) |  82672.647000 /1000 (82378,82587,82882) |
| OV(256,112,44)-pkc-skc |  1624572.420000 /100 (1621942,1622300,1622852) |  1032452.294000 /1000 (1030104,1030357,1030938) |  83443.233000 /1000 (83183,83359,83601) |
| OV(16,160,64)-classic |  3319456.020000 /100 (3314186,3318074,3319959) |  97797.817000 /1000 (92028,92240,92467) |  46291.899000 /1000 (44979,46205,47461) |
| OV(16,160,64)-pkc |  3292346.000000 /100 (3286803,3290909,3293951) |  97779.375000 /1000 (92015,92210,92455) |  97954.083000 /1000 (96216,97835,99629) |
| OV(16,160,64)-pkc-skc |  3315055.240000 /100 (3311437,3313696,3316969) |  2061408.996000 /1000 (2051077,2051666,2059528) |  97945.186000 /1000 (96209,97842,99527) |
| OV(256,184,72)-classic |  9449190.310000 /100 (9444228,9446517,9451209) |  187648.202000 /1000 (185977,186408,186826) |  184563.681000 /1000 (184198,184468,184781) |
| OV(256,184,72)-pkc |  9379581.140000 /100 (9375090,9376139,9379688) |  187703.953000 /1000 (185236,185666,186122) |  326970.536000 /1000 (326382,326898,327402) |
| OV(256,184,72)-pkc-skc |  9355994.830000 /100 (9349825,9350857,9355170) |  6136271.665000 /1000 (6129152,6136255,6137445) |  327715.097000 /1000 (327094,327438,327802) |
| OV(256,244,96)-classic |  28023508.630000 /100 (28011865,28015965,28023282) |  390875.404000 /1000 (381760,383246,396969) |  371866.895000 /1000 (371325,371685,372115) |
| OV(256,244,96)-pkc |  27723770.550000 /100 (27896749,27900801,27907386) |  400366.624000 /1000 (396164,396916,398035) |  700296.918000 /1000 (699826,700126,700498) |
| OV(256,244,96)-pkc-skc |  26806866.590000 /100 (26460078,26463880,26810831) |  15748054.154000 /1000 (15738043,15739151,15746384) |  712198.299000 /1000 (700283,702763,723184) |


7. neon implementation on ARM Cortex-A72(v8) @ 1.8GHz (Raspberry pi 4, aarch64, no aes extension)

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(256,112,44)-classic |  11447062.010000 /100 (11176918,11184344,11208819) |  244077.253000 /1000 (242099,242940,243760) |  147341.156000 /1000 (146081,146685,147470) |
| OV(256,112,44)-pkc |  11290613.720000 /100 (11117635,11131516,11146401) |  268508.709000 /1000 (260695,264781,268529) |  4125319.717000 /1000 (4109388,4126521,4133232) |
| OV(256,112,44)-pkc-skc |  11302040.950000 /100 (11129025,11136554,11146000) |  7882870.765000 /1000 (7859526,7862072,7873376) |  4109383.312000 /1000 (4098748,4108895,4113446) |
| OV(16,160,64)-classic |  28942157.280000 /100 (28591696,28639886,28690325) |  562007.368000 /1000 (527768,532316,539351) |  159074.915000 /1000 (153290,157286,162604) |
| OV(16,160,64)-pkc |  28578965.250000 /100 (28353429,28367829,28395352) |  701990.586000 /1000 (654270,662720,673771) |  5686087.255000 /1000 (5589174,5692720,5801705) |
| OV(16,160,64)-pkc-skc |  26649746.810000 /100 (26417795,26434420,26474485) |  16516600.609000 /1000 (16461753,16471394,16492783) |  5648442.693000 /1000 (5553974,5655593,5749768) |
| OV(256,184,72)-classic |  71123246.520000 /100 (70429238,70654623,70857391) |  1652919.626000 /1000 (1626906,1634516,1647293) |  672599.794000 /1000 (658949,663105,679198) |
| OV(256,184,72)-pkc |  71682613.230000 /100 (70733132,71626169,72017098) |  1815652.921000 /1000 (1790287,1804480,1820980) |  19197873.688000 /1000 (19142028,19179875,19227303) |
| OV(256,184,72)-pkc-skc |  66616709.310000 /100 (66019791,66149624,66303014) |  44553190.786000 /1000 (44450846,44498639,44564996) |  19306855.111000 /1000 (19261945,19291968,19329401) |
| OV(256,244,96)-classic |  318787586.150000 /100 (316810836,318633305,320124093) |  3649125.260000 /1000 (3558106,3625405,3690246) |  1466174.201000 /1000 (1443974,1458819,1467210) |
| OV(256,244,96)-pkc |  318508215.390000 /100 (317644827,318012194,318604575) |  3755366.045000 /1000 (3674403,3743637,3806855) |  44085496.168000 /1000 (43990165,44063727,44137498) |
| OV(256,244,96)-pkc-skc |  325220761.510000 /100 (324439922,324840210,325436696) |  113243118.617000 /1000 (113098541,113198818,113320609) |  43958579.630000 /1000 (43843916,43930901,44034623) |


7. neon implementation on ARM Cortex-A72(v8) @ 1.8GHz, defining _4RAES_ macro (Raspberry pi 4, aarch64, no aes extension)

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(256,112,44)-classic |  9175266.770000 /100 (8980668,8995825,9016237) |  255004.748000 /1000 (251929,253248,255175) |  151370.315000 /1000 (149625,150556,151742) |
| OV(256,112,44)-pkc |  9070667.780000 /100 (8909533,8919141,8929522) |  264063.116000 /1000 (257112,260404,267297) |  1919274.618000 /1000 (1911401,1913378,1925387) |
| OV(256,112,44)-pkc-skc |  8849272.870000 /100 (8675587,8685240,8696074) |  5670543.699000 /1000 (5663482,5668785,5672108) |  1909559.255000 /1000 (1904604,1906022,1915039) |
| OV(16,160,64)-classic |  25527561.490000 /100 (25278440,25311746,25343235) |  584422.089000 /1000 (542240,548013,559998) |  162905.468000 /1000 (158084,160823,164080) |
| OV(16,160,64)-pkc |  25310463.770000 /100 (25100334,25109983,25133808) |  630704.279000 /1000 (580735,602733,620719) |  2636918.385000 /1000 (2593373,2642800,2688268) |
| OV(16,160,64)-pkc-skc |  23396759.850000 /100 (23188064,23202073,23222361) |  13274255.404000 /1000 (13219675,13227111,13241865) |  2597749.577000 /1000 (2552563,2599386,2647327) |
| OV(256,184,72)-classic |  58570635.020000 /100 (58142022,58276178,58497620) |  1692353.001000 /1000 (1666813,1675403,1688759) |  662787.823000 /1000 (649224,651694,670407) |
| OV(256,184,72)-pkc |  61401543.460000 /100 (60754673,60927075,61336584) |  1910368.830000 /1000 (1884257,1896182,1931615) |  9260012.671000 /1000 (9218039,9242424,9273862) |
| OV(256,184,72)-pkc-skc |  58148223.570000 /100 (57442112,57810441,58113698) |  37277120.204000 /1000 (37187430,37228110,37283143) |  9621879.815000 /1000 (9598709,9615982,9642439) |
| OV(256,244,96)-classic |  302021507.710000 /100 (301149153,301462055,301956422) |  3387819.988000 /1000 (3314815,3374750,3446791) |  1459711.739000 /1000 (1440325,1454040,1461334) |
| OV(256,244,96)-pkc |  297718990.680000 /100 (296781542,297219084,297722105) |  3611743.901000 /1000 (3541339,3604513,3666962) |  21085700.979000 /1000 (20992986,21091068,21156680) |
| OV(256,244,96)-pkc-skc |  302770118.450000 /100 (302179944,302614530,303056537) |  91120675.747000 /1000 (91024732,91100712,91171443) |  21338963.601000 /1000 (21306637,21347876,21383671) |
