[![Linux Build Status](https://travis-ci.org/tpm2-software/tpm2-tcti-uefi.svg?branch=master)](https://travis-ci.org/tpm2-software/tpm2-tcti-uefi)
[![codecov](https://codecov.io/gh/tpm2-software/tpm2-tcti-uefi/branch/master/graph/badge.svg)](https://codecov.io/gh/tpm2-software/tpm2-tcti-uefi)

# Overview
This is an implementation of a TCTI module for use with the TCG TPM2
Software Stack (TSS2) in the UEFI environment. This library is built as a
static archive `libtss2-tcti-uefi.a` suitable for linking with UEFI
applications.

# API Stability
Methods for hiding symbols in a static arcive from users are not portable
and so this implementation makes no effort to do so. This does not imply
that all symbols from the archive present part of a stable API. The only
stable API exposed is the TCTI as specified by the TCG. This is a single
function called `Tss2_Tcti_Uefi_Init`. All other symbols are subject to
change without notice.

# Build and Installation Instructions
Instructions to build and install `libtss2-tcti-uefi.a` are available in
the [INSTALL](INSTALL.md) file.

Note that for using tpm2-tss System API, you need to apply the following
patch (also for executing the integration tests):
```diff
diff --git a/src/tss2-sys/api/Tss2_Sys_SetCmdAuths.c b/src/tss2-sys/api/Tss2_Sys_SetCmdAuths.c
index eb34964e..205dcc2d 100644
--- a/src/tss2-sys/api/Tss2_Sys_SetCmdAuths.c
+++ b/src/tss2-sys/api/Tss2_Sys_SetCmdAuths.c
@@ -64,8 +64,12 @@ TSS2_RC Tss2_Sys_SetCmdAuths(
 
     /* We're going to have to move stuff around.
      * First move current cpBuffer down by the auth area size. */
-    memmove(ctx->cpBuffer + authSize + sizeof(UINT32),
-            ctx->cpBuffer, ctx->cpBufferUsedSize);
+    //memmove(ctx->cpBuffer + authSize + sizeof(UINT32),
+    //        ctx->cpBuffer, ctx->cpBufferUsedSize);
+    // Quirk since memmove goes berserk on efi aarch64
+    for (size_t i = ctx->cpBufferUsedSize; i > 0; i--) {
+        ctx->cpBuffer[i - 1 + authSize + sizeof(UINT32)] = ctx->cpBuffer[i - 1];
+    }
 
     /* Reset the auth size field */
     memset(ctx->cpBuffer, 0, sizeof(UINT32));
```

# Source Layout
├── .ci - files for executing during CI
├── docs - further documentation
├── example - sample code using UEFI TCG2 protocol and tpm2-tss libraries
├── lib - files for setting up integration tests
├── src  - source code and headers for `libtss2-tcti-uefi.a`
└── test - unit tests

# Getting in Touch
If you're looking to discuss the source code in this project or get some
questions answered you should join the 01.org TPM2 mailing list:
  - https://lists.linuxfoundation.org/mailman/listinfo/tpm2

# Test Cases
Test strategy and cases are documented in [docs/test.md](docs/test.md).

# Examples
Several example UEFI executables are provided for reference and documented
in [docs/example.md](docs/example.md).
