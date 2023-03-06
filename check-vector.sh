#!/bin/sh

rm -rf *.rsp
./PQCgenKAT_sign
# make sure sha256sum is installed
sha256sum *.rsp || exit 1
hash=$(sha256sum  *.rsp | awk '{print $1}')
hashparam="$hash $1 $2"
if grep -q "$hashparam" SHA256SUMS
then
    echo OK
else
    echo NOT OKAY
    exit 1
fi