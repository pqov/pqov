#!/bin/sh

rm -rf *.rsp
./PQCgenKAT_sign
hash=$(sha256sum  *.rsp | awk '{print $1}')
hashparam="$hash $1 $2"
if grep -q "$hashparam" SHA256SUMS
then
    echo OK
else
    echo NOT OKAY
    exit 1
fi