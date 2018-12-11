#!/usr/bin/env sh

TMP_DIR=${TMP_DIR:-"/tmp/swtpm2"}
mkdir -p ${TMP_DIR}
swtpm socket --tpm2 \
    --tpmstate dir=${TMP_DIR} \
    --ctrl type=unixio,path=${TMP_DIR}/tpm-sock \
    --log level=20 2>&1 | tee ${TMP_DIR}/swtpm.log
