#!/usr/bin/env bash
# SPDX-License-Identifier: BSD-2

set -u
set -x

echo "Called: $@"

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
    $0 --ovmf=/usr/share/ovmf/OVMF.fd
       --startup-template=/path/to/template
       --preserve-tmp
       --tmp-dir=/tmp/foo
        TEST-SCRIPT [TEST-SCRIPT-ARGUMENTS]
END
}
while test $# -gt 0; do
    case $1 in
    --help) print_usage; exit $?;;
    -a|--arch) ARCH=$2; shift;;
    -a=*|--arch=*) ARCH="${1#*=}";;
    -o|--ovmf) OVMF_FD=$2; shift;;
    -o=*|--ovmf=*) OVMF_FD="${1#*=}";;
    -s|--startup-template) STARTUP_TEMPLATE=$2; shift;;
    -s=*|--startup-template=*) STARTUP_TEMPLATE="${1#*=}";;
    -p|--preserve-tmp) PRESERVE_TMP=yes;;
    -t|--tmp-dir) TMP_DIR=$2; shift;;
    -t=*|--tmp-dir=*) TMP_DIR="${1#*=}";;
    --) shift; break;;
    -*) usage_error "invalid option: '$1'";;
     *) break;;
    esac
    shift
done

TEST_EXEC=$1
TEST_NAME=$(basename $1)
shift
TEST_ARGS=$@
ARCH=${ARCH:-x86_64}
OVMF_FD=${OVMF_FD:-/usr/lib/elf_${ARCH}_efi.lds}
PRESERVE_TMP=${PRESERVE_TMP:-no}
TMP_DIR=${TMP_DIR:-$(mktemp --directory /tmp/tpm2-tcti-uefi_${TEST_NAME}.XXXXXXXX)}

if [ -z ${STARTUP_TEMPLATE} ]; then
    usage_error "--startup-template is required"
elif [ ! -f ${STARTUP_TEMPLATE} ]; then
    usage_error "--startup-template must be a file"
fi

if [ -z "${TEST_EXEC}" ]; then
    usage_error "missing test executable"
elif [ ! -f "$TEST_EXEC" ]; then
    usage_error "test executable must be a file"
fi
cp ${TEST_EXEC} ${TMP_DIR}
sed -e "s/[@]command[@]/${TEST_NAME}/" ${STARTUP_TEMPLATE} > ${TMP_DIR}/startup.nsh

swtpm socket --tpm2 --tpmstate dir=${TMP_DIR} \
    --ctrl type=unixio,path=${TMP_DIR}/tpm-sock \
    --log file=${TMP_DIR}/${TEST_NAME}_swtpm.log,level=20 &

QEMU_PARAMS="
    -nographic
    --bios ${OVMF_FD}
    -drive file=fat:rw:${TMP_DIR}
    -net none
    -chardev socket,id=chrtpm0,path=${TMP_DIR}/tpm-sock
    -tpmdev emulator,id=tpm0,chardev=chrtpm0"

if [ "${ARCH}" = "aarch64" ]; then
    QEMU_PARAMS+="
        -machine virt
        -cpu max
        -device tpm-tis-device,tpmdev=tpm0"
else if [ "${ARCH}" = "x86_64" ]; then
    QEMU_PARAMS+="
        -machine virt
        -device tpm-tis,tpmdev=tpm0"
else
    echo "Unsupported architecture ${ARCH}"
    ret=1
    goto done
fi
fi

echo "Starting $(which qemu-system-${ARCH}) with params: ${QEMU_PARAMS}"

timeout --preserve-status 30s unbuffer qemu-system-${ARCH} \
    ${QEMU_PARAMS} \
    | tee ${TMP_DIR}/${TEST_NAME}_qemu.log \
    | grep 'Reset with Success'
ret=$?

echo "=== QEMU Log ==="
cat ${TMP_DIR}/${TEST_NAME}_qemu.log
echo "=== swtpm Log ==="
cat ${TMP_DIR}/${TEST_NAME}_swtpm.log

if [ ${PRESERVE_TMP} == 'no' ]; then
    rm -rf ${TMP_DIR}
fi
exit $ret