#!/usr/bin/env sh

TMP_DIR=${TMP_DIR:-"/tmp/swtpm2"}
OVMF_IMG=${OVMF_IMG:-"/usr/share/ovmf/OVMF.fd"}
ROOTFS_DIR=${ROOTFS_DIR:-"./example"}
VNC_IP=${VNC_IP:-"127.0.0.1"}
VNC_NUM=${VNC_NUM:-"0"}
qemu-system-x86_64 \
    -vnc ${VNC_IP}:${VNC_NUM} \
    --bios ${OVMF_IMG} \
    -drive file=fat:rw:${ROOTFS_DIR} \
    -net none \
    -chardev "socket,id=chrtpm0,path=${TMP_DIR}/tpm-sock" \
    -tpmdev 'emulator,id=tpm0,chardev=chrtpm0' \
    -device 'tpm-tis,tpmdev=tpm0'
