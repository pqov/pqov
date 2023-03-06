

workflow_test = """name: Test $NAME

on:
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  test:
    name: Ubuntu ${{ matrix.impl }} ${{ matrix.cc }}
    runs-on: ubuntu-latest
    strategy:
      matrix:
        cc:
          - gcc
          - clang
        impl:
          - ref
          - avx2
          - ssse3
          - amd64
    steps:
    - uses: actions/checkout@v3
    - name: test
      run: make PARAM=$PARAM VARIANT=$VARIANT PROJ=${{ matrix.impl }} test
      env:
        CC: ${{ matrix.cc }}
    - name: asan
      run: |
        make clean
        make ASAN=1 PARAM=$PARAM VARIANT=$VARIANT PROJ=${{ matrix.impl }} test
      env:
        CC: ${{ matrix.cc }}
    - name: ubsan
      run: |
        make clean
        make UBSAN=1 PARAM=$PARAM VARIANT=$VARIANT PROJ=${{ matrix.impl }} test
      env:
        CC: ${{ matrix.cc }}
  valgrind:
    name: Valgrind constant-time check ${{ matrix.impl }} 
    runs-on: ubuntu-latest
    strategy:
      matrix:
        impl:
          - ref
          - avx2
          - ssse3
          - amd64
    steps:
    - uses: actions/checkout@v3
    - name: Install Valgrind
      run: sudo apt install valgrind
    - name: valgrind
      run: |
        make VALGRIND=1 PARAM=$PARAM VARIANT=$VARIANT PROJ=${{ matrix.impl }} sign_api-test
        valgrind --error-exitcode=1 --exit-on-first-error=yes --leak-check=yes ./sign_api-test
  test-macos:
    name: MacOS ${{ matrix.impl }} ${{ matrix.cc }}
    strategy:
      matrix:
        cc:
          - clang  # XCode (Apple LLVM/Clang)
        impl:
          - ref
          - avx2
          - ssse3
          - amd64
    runs-on: macos-latest
    steps:
    - uses: maxim-lobanov/setup-xcode@v1
      with:
        xcode-version: latest-stable
    - uses: actions/checkout@v3
    - name: Install OpenSSL
      run: |
        brew install openssl@3
        export LDFLAGS="-L/usr/local/opt/openssl@1.1/lib"
        export CFLAGS="-I/usr/local/opt/openssl@1.1/include"
    - name: Set up compiler
      run: 'export CC=${{ matrix.cc }}'
    - name: test
      run: make PARAM=$PARAM VARIANT=$VARIANT PROJ=${{ matrix.impl }} test
      env:
        CC: ${{ matrix.cc }}
    - name: asan
      run: |
        make clean
        make ASAN=1 PARAM=$PARAM VARIANT=$VARIANT PROJ=${{ matrix.impl }} test
      env:
        CC: ${{ matrix.cc }}
    - name: ubsan
      run: |
        make clean
        make UBSAN=1 PARAM=$PARAM VARIANT=$VARIANT PROJ=${{ matrix.impl }} test
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
    name: ${{ matrix.impl }} ${{ matrix.cc }}
    runs-on: ubuntu-latest
    strategy:
      matrix:
        cc:
          - gcc
          - clang
        impl:
          - ref
          - avx2
          - ssse3
          - amd64
    steps:
    - uses: actions/checkout@v3
    - name: test
      run: make KAT=1 PARAM=$PARAM VARIANT=$VARIANT PROJ=${{ matrix.impl }} check-NISTKAT
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
