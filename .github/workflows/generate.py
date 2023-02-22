

workflow = """name: Test $NAME

on:
  push:
    branches: [ "master" ]
  pull_request:
    branches: [ "master" ]

jobs:
  build:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v3
    - name: test
      run: make PARAM=$PARAM VARIANT=$VARIANT test
"""

def paramToName(p):
    if p == 1:
        return "OV16_160_64"
    elif p == 2:
        return "OV256_96_64"
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


for param in [1,2,3,4,5]:
    for variant in [1,2,3]:

        p = paramToName(param)
        v = variantToName(variant)
        name = f"{p}_{v}"
        w = workflow.replace("$PARAM", str(param)).replace("$VARIANT", str(variant)).replace("$NAME", name)
        with open(f"test_{name}.yml", "w") as f:
            f.write(w)