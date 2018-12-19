#!/usr/bin/env bash

sh lib/swtpm2.sh &
sed -i 's/-vnc.*/-nographic \\/g' lib/qemu-tpm2.sh
timeout 30s unbuffer sh lib/qemu-tpm2.sh | tee qemu_example.log
exec grep -q 'Tss2_Sys_GetCapability returned TSS2_RC_SUCCESS' qemu_example.log
