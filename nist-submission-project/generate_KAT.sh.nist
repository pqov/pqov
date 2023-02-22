#!/bin/sh

B=$1
[ -z $B] && B="KAT"

PARAM="Is"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="Is_pkc"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="Is_pkc_skc"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="Ip"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="Ip_pkc"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="Ip_pkc_skc"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="III"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="III_pkc"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="III_pkc_skc"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="V"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="V_pkc"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

PARAM="V_pkc_skc"
TAR="$B/$PARAM"
make clean; make PROJ=$PARAM KAT=1
./PQCgenKAT_sign
mkdir -p $TAR
cp -r PQCsignKAT_* $TAR
make clean

