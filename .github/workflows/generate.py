

workflow_test = """name: Test $NAME

on:
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:
    name: ${{ matrix.cc }}
    runs-on: ubuntu-latest
    strategy:
      matrix:
        cc:
          - gcc
          - clang
    steps:
    - uses: actions/checkout@v3
    - name: test
      run: make PARAM=$PARAM VARIANT=$VARIANT test
      env:
        CC: ${{ matrix.cc }}
    - name: asan
      run: |
        make clean
        make ASAN=1 PARAM=$PARAM VARIANT=$VARIANT test
      env:
        CC: ${{ matrix.cc }}
    - name: ubsan
      run: |
        make clean
        make UBSAN=1 PARAM=$PARAM VARIANT=$VARIANT test
      env:
        CC: ${{ matrix.cc }}
    - name: memsan
      run: |
        make clean
        make MEMSAN=1 PARAM=$PARAM VARIANT=$VARIANT test
      env:
        CC: ${{ matrix.cc }}
"""

workflow_nistkat = """name: NISTKAT $NAME

on:
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:
    name: ${{ matrix.cc }}
    runs-on: ubuntu-latest
    strategy:
      matrix:
        cc:
          - gcc
          - clang
    steps:
    - uses: actions/checkout@v3
    - name: test
      run: make KAT=1 PARAM=$PARAM VARIANT=$VARIANT check-NISTKAT
      env:
        CC: ${{ matrix.cc }}
"""

def paramToName(p):
    if p == 1:
        return "OV16_160_64"
    elif p == 3:
        return "OV256_112_44"
    elif p == 4:
        return "OV256_184_72"
    elif p == 5:
        return "OV256_244_96"

def variantToName(v):
    if v == 1:
        return "classic"
    elif v == 2:
        return "pkc"
    elif v == 3:
        return "pkc_skc"


for param in [1,3,4,5]:
    for variant in [1,2,3]:

        p = paramToName(param)
        v = variantToName(variant)
        name = f"{p}_{v}"
        w = workflow_test.replace("$PARAM", str(param)).replace("$VARIANT", str(variant)).replace("$NAME", name)
        with open(f"test_{name}.yml", "w") as f:
            f.write(w)

        w = workflow_nistkat.replace("$PARAM", str(param)).replace("$VARIANT", str(variant)).replace("$NAME", name)
        with open(f"nistkat_{name}.yml", "w") as f:
            f.write(w)
