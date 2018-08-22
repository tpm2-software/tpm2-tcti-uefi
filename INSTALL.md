This file contains instructions for building and installing tss2-tcti-uefi.

# Dependencies
* GNU Autoconf
* GNU Automake
* C compiler
* linker
* gnu-efi

# Build Instructions
> ./bootstrap && ./configure && make

# tpm2-tss Libraries
Due to limitations in the current tpm2-tss build implementation we cannot
link a UEFI application against the archives it produces. As a short-term
work around for this we have the tpm2-tss source code set up as a submodule
in the tpm2-uefi git repo. We build the libtss2-mu and libtss2-sys libraries
with custom build rules / flags.

These two libraries are built as part of the default make target. The output
is written to the `$(srcdir)/src` directory and the suffix `-uefi` is
appended to the library name before the `.a` suffix. These libraries are
installed as part of the `install` make target.

# tss2-tcti-uefi
The UEFI TCTI module is built as a static archive and is built as part of
the default make target. This library is written to `$(srcdir)/src` and is
installed by the `install` target.
