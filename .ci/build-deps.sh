#!/usr/bin/env sh

set -e

usage_error ()
{
    echo "$0: $*" >&2
    print_usage >&2
    exit 1
}

print_usage ()
{
    cat <<END
Usage:
    $0 --prefix=/usr/local --workdir=/path/to/dir
END
}

while test $# -gt 0; do
    case $1 in
    -h|--help) print_usage; exit $?;;
    -p|--prefix) PREFIX=$2; shift;;
    -p=*|--prefix=*) PREFIX="${1#*=}";;
    -t|--tss2-config-site) TSS2_CONFIG_SITE=$2; shift;;
    -t=*|--tss2-config-site=*) TSS2_CONFIG_SITE="${1#*=}";;
    -w|--workdir) WORKDIR=$2; shift;;
    -w=*|--workdir=*) WORKDIR="${1#*=}";;
    --) shift; break;;
    -*) usage_error "invalid option: '$1'";;
     *) break;;
    esac
    shift
done

MAKE=${MAKE:-"make --jobs=$(($(nproc)*3/2))"}
PREFIX=${PREFIX:-"/usr/local"}
WORKDIR=${WORKDIR:-"$(mktemp --directory --tmpdir=/tmp tmp.XXXXXXXXXX)"}
STARTDIR=$(pwd)
TSS2_CONFIG_SITE=${TSS2_CONFIG_SITE:-"${STARTDIR}/lib/tss2-sys_config.site"}

if [ ! -d "${WORKDIR}" ]; then
    mkdir "${WORKDIR}"
fi
cd ${WORKDIR}

wget https://download.01.org/tpm2/autoconf-archive-2017.09.28.tar.xz
sha256sum autoconf-archive-2017.09.28.tar.xz | \
  grep -q 5c9fb5845b38b28982a3ef12836f76b35f46799ef4a2e46b48e2bd3c6182fa01
tar xJf autoconf-archive-2017.09.28.tar.xz
cd autoconf-archive-2017.09.28
./configure --prefix=${PREFIX}
$MAKE
sudo $MAKE install
cd -

git clone --branch master --single-branch \
  https://github.com/tpm2-software/tpm2-tss.git
cd tpm2-tss
./bootstrap
CONFIG_SITE="${TSS2_CONFIG_SITE}" ./configure --prefix=${PREFIX} \
  --disable-defaultflags --disable-doxygen-doc --disable-tcti-device \
  --disable-tcti-mssim --disable-esapi --with-maxloglevel=none
$MAKE
sudo $MAKE install
cd -

cd ${STARTDIR}
rm -rf "${WORKDIR}"
