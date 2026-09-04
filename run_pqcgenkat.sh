#!/bin/sh

# usage: ./run_pqcgenkat.sh [-a] [KATDIR] [MAKE_FLAGS]
# With -a, each KAT is also archived as <config>.PQCsignKAT_*.rsp.xz, so that
# the names stay unambiguous where they share one flat namespace.

ARCHIVE=""
while getopts a opt; do
    case $opt in
        a) ARCHIVE=1 ;;
        *) echo "usage: $0 [-a] [KATDIR] [MAKE_FLAGS]" >&2; exit 1 ;;
    esac
done
shift $((OPTIND - 1))

rm -rf SHA256SUMS

B=$1
[ -z "$B" ] && B="KAT"

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

[ -z "$ARCHIVE" ] && exit 0

# One archive per parameter set, plus SHA256SUMS.release: digests of the
# uncompressed .rsp under the archive names, for sha256sum -c after unpacking.
# The SHA256SUMS that check-vector.sh greps keeps its own format.
rm -f $B/*.rsp.xz $B/SHA256SUMS.release
for TAR in $B/*/; do
    name=$(basename $TAR)
    rsp=$(ls $TAR/PQCsignKAT_*.rsp 2>/dev/null)
    [ -f "$rsp" ] || { echo "$TAR does not hold exactly one PQCsignKAT_*.rsp" >&2; exit 1; }
    asset="$name.$(basename $rsp)"
    xz -9 -c $rsp > $B/$asset.xz
    sha256sum $rsp | awk -v n="$asset" '{print $1 "  " n}' >> $B/SHA256SUMS.release
    echo "archived $asset.xz"
done
