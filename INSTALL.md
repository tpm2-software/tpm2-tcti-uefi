This file contains instructions for building and installing tss2-tcti-uefi.

# Dependencies
* GNU Autoconf
* GNU Automake
* C compiler
* linker
* gnu-efi
* tpm2-tss

# Build Instructions
> ./bootstrap && ./configure && make

# tss2-tcti-uefi
The UEFI TCTI module is built as a static archive and is built as part of
the default make target. This library is written to `$(srcdir)/src` and is
installed by the `install` target.
