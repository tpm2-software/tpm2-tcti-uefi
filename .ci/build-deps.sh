#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2
set -e
set -x

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
TSS2_CONFIG_SITE=${TSS2_CONFIG_SITE:-"${STARTDIR}/.ci/tss2-sys_config.site"}

export PREFIX
export PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig"
export LD_LIBRARY_PATH="${PREFIX}/lib"
export PATH="${PREFIX}/bin:${PREFIX}/sbin:${PATH}"
export MANPATH="${PREFIX}/share/man:/usr/share/man"
export CPPFLAGS="-I${PREFIX}/include"
export LDFLAGS="-L${PREFIX}/lib"


if [ ! -d "${WORKDIR}" ]; then
    mkdir -p "${WORKDIR}"
fi
cd ${WORKDIR}

if [ ! -d "${PREFIX}" ]; then
    mkdir -p "${PREFIX}"
fi

git clone --branch master --single-branch --depth=1 \
  https://github.com/stefanberger/libtpms
cd libtpms
./autogen.sh --with-openssl --with-tpm2 --prefix=${PREFIX} \
  CPPFLAGS="${CPPFLAGS}" LDFLAGS="${LDFLAGS}"
$MAKE
$MAKE install
cd -

# swtpm isn't compatible with clang
CC_BAK=$CC
CC=gcc
git clone --branch master --single-branch --depth=1 \
  https://github.com/stefanberger/swtpm
cd swtpm
./autogen.sh --prefix=${PREFIX} --without-seccomp \
  CPPFLAGS="${CPPFLAGS}" LDFLAGS="${LDFLAGS}"
$MAKE
$MAKE install
cd -
CC=$CC_BAK

git clone --branch master --single-branch --depth=1 \
  https://github.com/tpm2-software/tpm2-tss.git
cd tpm2-tss
./bootstrap
CONFIG_SITE="${TSS2_CONFIG_SITE}" ./configure --prefix=${PREFIX} \
  --disable-defaultflags --disable-doxygen-doc --disable-tcti-device \
  --disable-tcti-mssim --disable-tcti-spidev --disable-esys \
  --disable-fapi --disable-policy --with-maxloglevel=none \
  CPPFLAGS="${CPPFLAGS}" LDFLAGS="${LDFLAGS}"
$MAKE
$MAKE install
cd -

QEMU_VERSION=9.1.0
QEMU_DIR=qemu-$QEMU_VERSION
QEMU_SRC=${QEMU_DIR}.tar.xz
QEMU_SRC_SHA256=816b7022a8ba7c2ac30e2e0cf973e826f6bcc8505339603212c5ede8e94d7834
wget "https://download.qemu.org/${QEMU_SRC}"
sha256sum "${QEMU_SRC}" | grep -q "${QEMU_SRC_SHA256}"
if [ ! $? -eq 0 ]; then
    echo "hash mismatch on qemu tarball"
    exit 1
fi
tar axf ${QEMU_SRC}
cd ${QEMU_DIR}
./configure --prefix=${PREFIX} --enable-tpm --disable-kvm \
  --target-list=aarch64-softmmu,x86_64-softmmu
$MAKE
$MAKE install
cd -

GNUEFI_VERSION=3.0.18
GNUEFI_NAME=gnu-efi-${GNUEFI_VERSION}
GNUEFI_BZ2=$GNUEFI_NAME.tar.bz2
GNUEFI_BZ2_SHA256=7f212c96ee66547eeefb531267b641e5473d7d8529f0bd8ccdefd33cf7413f5c
wget "https://downloads.sourceforge.net/project/gnu-efi/${GNUEFI_BZ2}"
sha256sum "${GNUEFI_BZ2}" | grep -q "${GNUEFI_BZ2_SHA256}"
if [ ! $? -eq 0 ]; then
    echo "hash mismatch on gnu-efi tarball"
    exit 1
fi
tar axf ${GNUEFI_BZ2}
cd ${GNUEFI_NAME}
make
$MAKE PREFIX=${PREFIX} install
cd -

cd ${STARTDIR}
rm -rf "${WORKDIR}"
