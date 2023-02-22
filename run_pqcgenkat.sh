#!/bin/sh

B=$1
[ -z $B] && B="KAT"

TAR="$B/Is"
make clean; make $2 PARAM=1 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/Is-pkc"
make clean; make $2 VARIANT=2 PARAM=1 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/Is-pkc-skc"
make clean; make $2 VARIANT=3 PARAM=1 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/Ip"
make clean; make $2 PARAM=3 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/Ip-pkc"
make clean; make $2 VARIANT=2 PARAM=3 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/Ip-pkc-skc"
make clean; make $2 VARIANT=3 PARAM=3 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/III"
make clean; make $2 PARAM=4 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/III-pkc"
make clean; make $2 VARIANT=2 PARAM=4 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/III-pkc-skc"
make clean; make $2 VARIANT=3 PARAM=4 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/V"
make clean; make $2 PARAM=4 KAT=5 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/V-pkc"
make clean; make $2 VARIANT=2 PARAM=5 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

TAR="$B/V-pkc-skc"
make clean; make $2 VARIANT=3 PARAM=5 KAT=1 PQCgenKAT_sign
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

