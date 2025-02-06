  
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


## Instructions for testing and generating KATs

Type **make**    
```
make
```
It will generate an executable **sign_api-test** testing API functions (crypto_keygen(), crypto_sign(), and crypto_verify()).  

### **Options for Parameters:**

The source code of all supporting parameters is stored in subdirectories of the parameter names.
The default setting is compiling **Ip**.  
We use **PROJ** parameters for compiling other parameters.  
For example:  
```
make PROJ=Is_pkc
```
```
make PROJ=III_pkc_skc
```

### **Generating KAT:**
Use the following script:  
```
source ./generate_KAT.sh
```

