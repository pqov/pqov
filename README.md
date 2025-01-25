  
# OV signature

This project contains the reference and optimized implementations of the oil and vinegar (OV) signature system.

## Licence

Public domain unless stated differently at the top of each file.

## Parameters

| Parameter    | signature size | pk size  | sk size | pkc size | compressed-sk size |  
|:-----------: |:--------------:|--------- |---------|------------|--------------------|
|GF(16),160,64 | 96             |412,160   |348,704  |66,576      | 48                 |
|GF(256),112,44| 128            |278,432   |237,896  |43,576      | 48                 |
|GF(256),184,72| 200            |1,225,440 |1,044,320|189,232     | 48                 |
|GF(256),244,96| 260            |2,869,440 |2,436,704|446,992     | 48                 |

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
| OV(16,160,64)-classic |  3393202.048000 /1000 (3389693,3391967,3395605) |  79649.492100 /10000 (74491,74633,74814) |  45974.790500 /10000 (44777,45908,47078) |
| OV(16,160,64)-pkc |  3366171.190000 /1000 (3359391,3360648,3363265) |  79637.568200 /10000 (74612,74757,74938) |  138389.038700 /10000 (136221,138541,140629) |
| OV(16,160,64)-pkc-skc |  3399677.154000 /1000 (3390185,3393812,3397537) |  2094305.040600 /10000 (2088941,2089131,2089364) |  138383.778500 /10000 (136248,138496,140612) |
| OV(256,112,44)-classic |  1797428.261000 /1000 (1791766,1793119,1794443) |  55554.494500 /10000 (55244,55320,55413) |  49796.923700 /10000 (49550,49719,49976) |
| OV(256,112,44)-pkc |  1779785.786000 /1000 (1774902,1775826,1776591) |  55520.226000 /10000 (55203,55289,55391) |  113035.876400 /10000 (112739,112934,113235) |
| OV(256,112,44)-pkc-skc |  1781368.708000 /1000 (1773490,1774748,1775610) |  1056925.781800 /10000 (1056437,1056617,1056802) |  113889.868100 /10000 (113626,113805,114074) |
| OV(256,184,72)-classic |  9842855.186000 /1000 (9833331,9836359,9842447) |  149716.423600 /10000 (148561,149280,149439) |  189911.501600 /10000 (189580,189837,190151) |
| OV(256,184,72)-pkc |  9808371.327000 /1000 (9801978,9803637,9805500) |  148068.639000 /10000 (147459,147564,147708) |  462334.340400 /10000 (461835,462306,462730) |
| OV(256,184,72)-pkc-skc |  9761487.225000 /1000 (9749338,9751198,9753375) |  6353695.307200 /10000 (6353126,6353401,6353665) |  462173.811200 /10000 (461515,461896,462369) |
| OV(256,244,96)-classic |  28294004.860000 /1000 (28275650,28286979,28297149) |  298569.667200 /10000 (293517,293826,294532) |  376079.556800 /10000 (375692,376000,376372) |
| OV(256,244,96)-pkc |  26889989.542000 /1000 (26710677,26743866,26754034) |  310212.719500 /10000 (308849,309159,309459) |  1012621.870400 /10000 (1012091,1012482,1012937) |
| OV(256,244,96)-pkc-skc |  26750917.303000 /1000 (26661383,26663940,26674888) |  15836867.328600 /10000 (15829550,15830169,15831069) |  1015436.129700 /10000 (1010911,1011331,1012047) |


6. neon implementation on Apple M1, defining _4RAES_ macro

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(16,160,64)-classic |  3354871.311000 /1000 (3348860,3349853,3351112) |  79175.906800 /10000 (74372,74515,74696) |  45800.548600 /10000 (44629,45711,46863) |
| OV(16,160,64)-pkc |  3327670.425000 /1000 (3321012,3324331,3326806) |  79323.975100 /10000 (74366,74503,74679) |  97395.223100 /10000 (95793,97391,98984) |
| OV(16,160,64)-pkc-skc |  3352449.915000 /1000 (3342295,3349000,3351955) |  2050209.412400 /10000 (2044850,2045042,2045331) |  97323.289600 /10000 (95756,97325,98867) |
| OV(256,112,44)-classic |  1766518.643000 /1000 (1761364,1762817,1763931) |  55363.380500 /10000 (55097,55175,55267) |  49893.352300 /10000 (49644,49808,50070) |
| OV(256,112,44)-pkc |  1749851.720000 /1000 (1745637,1746623,1747825) |  55542.583600 /10000 (55244,55329,55431) |  83097.113800 /10000 (82796,83021,83311) |
| OV(256,112,44)-pkc-skc |  1751771.015000 /1000 (1746535,1748646,1750924) |  1027002.902200 /10000 (1026583,1026701,1026841) |  84001.746000 /10000 (83738,83920,84191) |
| OV(256,184,72)-classic |  9694194.041000 /1000 (9688797,9690791,9692931) |  149529.498900 /10000 (148460,149120,149299) |  189887.573600 /10000 (189566,189817,190140) |
| OV(256,184,72)-pkc |  9645633.052000 /1000 (9639195,9640984,9643113) |  147986.401300 /10000 (147396,147524,147666) |  330573.232900 /10000 (330084,330463,330877) |
| OV(256,184,72)-pkc-skc |  9650337.274000 /1000 (9644063,9645510,9648015) |  6222689.873100 /10000 (6221007,6221280,6221588) |  331661.876800 /10000 (330625,331074,331700) |
| OV(256,244,96)-classic |  27859451.168000 /1000 (27846303,27852840,27859809) |  299725.738900 /10000 (292736,293117,308054) |  376210.326900 /10000 (375814,376132,376506) |
| OV(256,244,96)-pkc |  26580995.262000 /1000 (26298820,26305292,26340264) |  309852.521500 /10000 (308127,308410,308684) |  707297.318200 /10000 (706823,707217,707643) |
| OV(256,244,96)-pkc-skc |  26643439.819000 /1000 (26289085,26298657,26354765) |  15526142.527500 /10000 (15522021,15522513,15523170) |  706816.234400 /10000 (704644,704986,705439) |


7. neon implementation on ARM Cortex-A72(v8) @ 1.8GHz (Raspberry pi 4, aarch64, no aes extension)

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(16,160,64)-classic |  29290685.950000 /500 (29239158,29269925,29310858) |  494872.201800 /10000 (458114,460655,470144) |  143442.171400 /10000 (139658,141528,143558) |
| OV(16,160,64)-pkc |  28936858.182000 /500 (28894743,28906183,28925222) |  1580336802.216000 /10000 (542228,549105,559817) |  5080532.816700 /10000 (4993594,5087095,5174934) |
| OV(16,160,64)-pkc-skc |  29483404.598000 /500 (29451741,29467684,29491076) |  1395940904.282800 /10000 (16401080,16413501,16435793) |  1844672822620759.000000 /10000 (4974918,5070253,5156379) |
| OV(256,112,44)-classic |  11188099.140000 /500 (11163154,11172204,11185089) |  247106.639500 /10000 (244375,245095,245897) |  143717.544900 /10000 (142618,142868,143161) |
| OV(256,112,44)-pkc |  11224762.366000 /500 (11186542,11193794,11198712) |  256224.640400 /10000 (251210,253324,255524) |  3678706.104100 /10000 (3668527,3681898,3684604) |
| OV(256,112,44)-pkc-skc |  11295081.128000 /500 (11219457,11229231,11318013) |  7621048.374000 /10000 (7616217,7617137,7618574) |  3674931.824400 /10000 (3666397,3677844,3679458) |
| OV(256,184,72)-classic |  66931801.010000 /500 (66731124,66871027,67029434) |  1557611.746800 /10000 (1520321,1542143,1587587) |  588840.363000 /10000 (571977,574080,589334) |
| OV(256,184,72)-pkc |  66570566.088000 /500 (66275754,66554826,66793786) |  1659039.631200 /10000 (1623087,1632966,1692327) |  1374057048.781200 /10000 (17122760,17161246,17200844) |
| OV(256,184,72)-pkc-skc |  64199301.720000 /500 (64056764,64147364,64275590) |  42820542.201000 /10000 (42746359,42794977,42855995) |  1336706755.118000 /10000 (17338010,17369004,17405721) |
| OV(256,244,96)-classic |  313822979.386000 /500 (313313865,313814250,314327646) |  3347921.603300 /10000 (3272056,3337536,3403568) |  1342670.939700 /10000 (1310429,1319092,1390676) |
| OV(256,244,96)-pkc |  306220128.888000 /500 (305215682,305700907,306292823) |  3324891.704800 /10000 (3245750,3316413,3385823) |  39345343.824300 /10000 (39295821,39337795,39385602) |
| OV(256,244,96)-pkc-skc |  312769807.848000 /500 (312264832,312729427,313148491) |  107318888.779000 /10000 (107227206,107305680,107381535) |  39389996.820300 /10000 (39326493,39390657,39452436) |


7. neon implementation on ARM Cortex-A72(v8) @ 1.8GHz, defining _4RAES_ macro (Raspberry pi 4, aarch64, no aes extension)

| Parameter    | Key-generation cycles | signing cycles | sign-opening cycles |
|:-----------: |        --------------:|      ---------:|           ---------:|
| OV(16,160,64)-classic |  26104042.366000 /500 (26055865,26079083,26109987) |  479892.969200 /10000 (445703,448188,454231) |  135681.459000 /10000 (131743,133721,136119) |
| OV(16,160,64)-pkc |  25712346.908000 /500 (25690772,25698880,25710821) |  550544.434100 /10000 (508517,517451,530845) |  2308786.814100 /10000 (2264483,2309671,2354513) |
| OV(16,160,64)-pkc-skc |  28350136.382000 /500 (28308338,28324760,28346176) |  13374690.195600 /10000 (13328127,13333557,13351565) |  2263162.810800 /10000 (2223998,2266233,2304816) |
| OV(256,112,44)-classic |  9491444.574000 /500 (9419551,9436542,9450911) |  257070.261600 /10000 (252719,254211,255892) |  153938.584600 /10000 (151079,152130,153527) |
| OV(256,112,44)-pkc |  9196501.852000 /500 (9181131,9191247,9197749) |  253417.475800 /10000 (248689,249910,252309) |  1678078.192600 /10000 (1671641,1672544,1686208) |
| OV(256,112,44)-pkc-skc |  9478628.272000 /500 (9466694,9473513,9481548) |  5631076.342600 /10000 (5625684,5627393,5637580) |  1678325.878400 /10000 (1673384,1674207,1683582) |
| OV(256,184,72)-classic |  2371867147.266000 /500 (57079160,57311639,57579767) |  1595025.065900 /10000 (1558627,1569429,1623583) |  587322.646100 /10000 (570475,572965,586951) |
| OV(256,184,72)-pkc |  56918872.966000 /500 (56663215,56890636,57122899) |  1758320.144800 /10000 (1720138,1729733,1797286) |  8319356.344400 /10000 (8282039,8318527,8349960) |
| OV(256,184,72)-pkc-skc |  56859414.722000 /500 (56676866,56815652,56969672) |  1844673000101042.500000 /10000 (34488140,34533235,34600256) |  8636187.722000 /10000 (8602234,8634365,8663370) |
| OV(256,244,96)-classic |  295923082.904000 /500 (295058951,295693403,296663039) |  3352007.747000 /10000 (3270463,3339648,3408099) |  1352412.647500 /10000 (1320262,1328232,1398580) |
| OV(256,244,96)-pkc |  36893485758928136.000000 /500 (282248561,282742682,283172951) |  3387885.507700 /10000 (3312617,3376153,3440469) |  18848531.373100 /10000 (18789243,18831724,18894871) |
| OV(256,244,96)-pkc-skc |  291461557.636000 /500 (290948203,291438637,291896182) |  1419459809.967000 /10000 (86648551,86727909,86825980) |  18629406.251200 /10000 (18521783,18602008,18724379) |
