#!/bin/sh

rm -rf SHA256SUMS

B=$1
[ -z $B] && B="KAT"

TAR="$B/Is"
make clean; make $2 PARAM=1 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 1 1 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/Is-pkc"
make $2 VARIANT=2 PARAM=1 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 1 2 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/Is-pkc-skc"
make $2 VARIANT=3 PARAM=1 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 1 3 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/Ip"
make $2 PARAM=3 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 3 1 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/Ip-pkc"
make $2 VARIANT=2 PARAM=3 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 3 2 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/Ip-pkc-skc"
make $2 VARIANT=3 PARAM=3 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 3 3 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/III"
make $2 PARAM=4 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 4 1 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/III-pkc"
make $2 VARIANT=2 PARAM=4 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 4 2 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/III-pkc-skc"
make $2 VARIANT=3 PARAM=4 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 4 3 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/V"
make $2 PARAM=5 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 5 1 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/V-pkc"
make $2 VARIANT=2 PARAM=5 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 5 2 '$TAR'"}' >> SHA256SUMS
make clean

TAR="$B/V-pkc-skc"
make $2 VARIANT=3 PARAM=5 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
sha256sum *.rsp | awk '{print $1 " 5 3 '$TAR'"}' >> SHA256SUMS
make clean

