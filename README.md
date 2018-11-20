# Overview
This is an implementation of a TCTI module for use with the TCG TPM2
Software Stack (TSS2) in the UEFI environment. This library is built as a
static archive `libtss2-tcti-uefi.a` suitable for linking with UEFI
applications.

# Build and Installation Instructions
Instructions to build and install `libtss2-tcti-uefi.a` are available in
the [INSTALL](INSTALL.md) file.

# Source Layout
├── example - sample code using UEFI TCG2 protocol and tpm2-tss libraries  
├── src  - source code and headers for `libtss2-tcti-uefi.a`  
└── test - unit tests

# Getting in Touch
If you're looking to discuss the source code in this project or get some
questions answered you should join the 01.org TPM2 mailing list:
https://lists.01.org/mailman/listinfo/tpm2.
We also have an IRC channel set up on [FreeNode](https://freenode.net/)
called \#tpm2.0-tss.

# Test Cases
Unit tests must be explicitly enabled using the configure script. Use the
`-help` option to the `configure` script for documentation. If enabled, unit
tests are executed using the `check` make target:
> make check
