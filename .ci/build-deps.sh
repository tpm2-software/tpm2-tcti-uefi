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

git clone --branch master --single-branch \
  https://github.com/stefanberger/libtpms
cd libtpms
./autogen.sh --with-openssl --with-tpm2 --prefix=${PREFIX}
$MAKE
sudo $MAKE install
cd -

# swtpm isn't compatible with clang
CC_BAK=$CC
CC=gcc
git clone --branch master --single-branch \
  https://github.com/stefanberger/swtpm
cd swtpm
./autogen.sh --prefix=${PREFIX}
$MAKE
sudo $MAKE install
cd -
CC=$CC_BAK

QEMU_SRC=qemu-3.1.0.tar.xz
QEMU_SRC_SHA256=6a0508df079a0a33c2487ca936a56c12122f105b8a96a44374704bef6c69abfc
wget "https://download.qemu.org/${QEMU_SRC}"
sha256sum "${QEMU_SRC}" | grep -q "${QEMU_SRC_SHA256}"
if [ ! $? -eq 0 ]; then
    echo "hash mismatch on qemu tarball"
    exit 1
fi
tar axf ${QEMU_SRC}
cd qemu-3.1.0
./configure --prefix=${PREFIX} --enable-tpm --disable-kvm \
  --target-list=x86_64-softmmu
$MAKE
sudo $MAKE install
cd -

GNUEFI_BZ2=gnu-efi-3.0.9.tar.bz2
GNUEFI_BZ2_SHA256=6715ea7eae1c7e4fc5041034bd3f107ec2911962ed284a081e491646b12277f0
wget "https://downloads.sourceforge.net/project/gnu-efi/${GNUEFI_BZ2}"
sha256sum "${GNUEFI_BZ2}" | grep -q "${GNUEFI_BZ2_SHA256}"
if [ ! $? -eq 0 ]; then
    echo "hash mismatch on gnu-efi tarball"
    exit 1
fi
tar axf ${GNUEFI_BZ2}
cd gnu-efi-3.0.9
make
sudo $MAKE PREFIX=${PREFIX} install
cd -

OVMF_RPM=edk2.git-ovmf-x64-0-20190131.914.g7a90895306.noarch.rpm
OVMF_RPM_SHA256=582975520720e318c210c6930d046929d35c703e4ec275e37c8e1e89737d345d
wget "https://www.kraxel.org/repos/jenkins/edk2/${OVMF_RPM}"
sha256sum "${OVMF_RPM}" | grep -q "${OVMF_RPM_SHA256}"
if [ ! $? -eq 0 ]; then
    echo "hash mismatch on OVMF rpm"
    exit 1
fi
rpm2cpio "${OVMF_RPM}" | cpio -i -d --no-absolute-filenames -v
OVMF_FD=$(find ./ -name 'OVMF-pure-efi.fd')
sudo mkdir /usr/share/ovmf
sudo cp ${OVMF_FD} /usr/share/ovmf/OVMF.fd

cd ${STARTDIR}
rm -rf "${WORKDIR}"
