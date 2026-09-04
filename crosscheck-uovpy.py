#!/usr/bin/env python3
"""Cross-check our KAT digests against uov-py, the independent Python
implementation at https://github.com/mjosaarinen/uov-py.

    git clone https://github.com/mjosaarinen/uov-py   # checked at 308f792
    pip install -r uov-py/requirements.txt            # pycryptodome
    ./crosscheck-uovpy.py uov-py            # every parameter set and variant
    ./crosscheck-uovpy.py uov-py 3 1        # just uov-Ip, classic

Run it from the top of this repository: SHA256SUMS is read from the working
directory.  A full run takes some forty minutes, a quarter of that if the
parameter sets are run in parallel.

uov-py is used unmodified: its UOV class takes (gf, n, m) as arguments, so the
parameter sets live here rather than in a patched copy of uov.py.  Both sides
write the same NIST .rsp text and drive the same DRBG, so the sha256 of 100
vectors is the digest SHA256SUMS records.

Needs Python >= 3.11 (uov.py relies on int.from_bytes() defaulting to big) and
pycryptodome.
"""

import sys

# PARAM and VARIANT numbers as the Makefile dispatches them.
PARAMS = {1: (16,  160, 64, 'uov-Is'),  3: (256, 119, 45, 'uov-Ip'),
          4: (256, 193, 72, 'uov-III'), 5: (256, 259, 96, 'uov-V')}
VARIANTS = {1: (False, False), 2: (True, False), 3: (True, True)}

if len(sys.argv) not in (2, 4):
    sys.exit(__doc__)
sys.path.insert(0, sys.argv[1])

from Crypto.Hash import SHA256
from kat_test import test_rsp
from uov import UOV

params = [int(sys.argv[2])] if len(sys.argv) == 4 else sorted(PARAMS)
variants = [int(sys.argv[3])] if len(sys.argv) == 4 else sorted(VARIANTS)

expect = {}
for line in open('SHA256SUMS'):
    digest, param, variant = line.split()[:3]
    expect[int(param), int(variant)] = digest

failed = 0
for p in params:
    for v in variants:
        gf, n, m, name = PARAMS[p]
        pkc, skc = VARIANTS[v]
        iut = UOV(gf=gf, n=n, m=m, pkc=pkc, skc=skc)
        kat = test_rsp(iut, katnum=100)
        digest = SHA256.new(kat.encode('ASCII')).hexdigest()
        want = expect.get((p, v))
        ok = digest == want
        failed += not ok
        print(f'PARAM={p} VARIANT={v} {name} {iut.katname}: '
              f'{"MATCH " + digest if ok else f"MISMATCH {digest} != {want}"}',
              flush=True)

sys.exit(1 if failed else 0)
