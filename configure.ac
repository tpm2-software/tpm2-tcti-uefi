# SPDK-License-Identifier: BSD-2
AC_INIT([tpm2-tcti-uefi],
        [0.0.0],
        [https://github.com/tpm2-software/tpm2-tcti-uefi/issues],
        [],
        [https://github.com/tpm2-software/tpm2-tcti-uefi])
# bring canonical names for build tripple, we need target_cpu
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AS_IF([test "x$target_cpu" = "xi386" -o "x$target_cpu" = "xi686"],
      [ARCH="ia32"
       EXTRA_CFLAGS=-DEFI_FUNCTION_WRAPPER],
      [test "x$target_cpu" = "xx86_64"],
      [ARCH="x86_64"],
      [AC_MSG_ERROR(["Unsupported target CPU: $target_cpu"])])
# standard c stuff
AC_PROG_CC
AC_PROG_RANLIB
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_PROG_AR([AC_MSG_ERROR([Missing required tool: ar])])
AC_CONFIG_FILES([Makefile])

# required programs (beyond compiler / linker)
AC_CHECK_TOOL([OBJCOPY], objcopy, [AC_MSG_ERROR([Missing required tool: objcopy])])

# unit test library
AC_ARG_ENABLE([unit],
              [AS_HELP_STRING([--enable-unit],
                   [build cmocka unit tests (default is no)])],
              [enable_unit=$enableval],
              [enable_unit=no])
AS_IF([test "x$enable_unit" != xno],
      [PKG_CHECK_MODULES([CMOCKA],
                         [cmocka >= 1.0],
                         [AC_DEFINE([HAVE_CMOCKA],
                                    [1],
                                    [cmocka is available])])])
AM_CONDITIONAL([UNIT], [test "x$enable_unit" != xno])

# integration test
AC_ARG_ENABLE([integration],
              [AS_HELP_STRING([--enable-integration],
                  [build and run integration tests (default is no)])],
              [enable_integration=$enableval],
              [enable_integration=no])
AS_IF([test "x$enable_integration" != xno],
      [AC_CHECK_PROG([SWTPM],
                     swtpm,
                     [yes],
                     [AC_MSG_ERROR([Missing required program: swtpm])])
       AC_CHECK_PROG([QEMU],
                     qemu-system-x86_64,
                     [yes],
                     [AC_MSG_ERROR([Missing required program: qemu-system-x86_64])])
       AC_ARG_WITH([ovmf],
                    AS_HELP_STRING([--with-ovmf=PATH],
                                   [Path to OVMF.fd]),
                   [AC_SUBST([OVMF_PATH],[$withval])],
                   [AC_SUBST([OVMF_PATH],[/usr/share/ovmf/OVMF.fd])])])
AM_CONDITIONAL([INTEGRATION], [test "x$enable_integration" != xno])

# directory where gnu-efi headers live
AC_ARG_WITH([efi-includedir],
             AS_HELP_STRING([--with-efi-includedir=DIR],
                            [Directory containing gnu-efi headers.]),
            [],
            [with_efi_includedir=/usr/include/efi])
EXTRA_CFLAGS+="-I${with_efi_includedir} -I${with_efi_includedir}/${ARCH}"
# this is really ugly but AC_CHECK_HEADER doesn't allow us to pass in
# compiler flags / search path
CFLAGS_TMP="$CFLAGS"
CFLAGS="$CFLAGS $EXTRA_CFLAGS"
AC_CHECK_HEADERS([efi.h efilib.h],
                 [],
                 [AC_MSG_ERROR([Missing gnu-efi headers.])],
                 [#include <efi.h>])
CFLAGS="$CFLAGS_TMP"

# path to linker script from gnu-efi
AC_ARG_WITH([efi-lds],
            AS_HELP_STRING([--with-efi-lds=LDS_PATH],[Path to gnu-efi lds file.]),
            [],
            [with_efi_lds="/usr/lib/elf_${ARCH}_efi.lds"])
AC_CHECK_FILE(["${with_efi_lds}"],
              [],
              [AC_MSG_ERROR([Missing file: ${with_efi_lds}.])])
EXTRA_LDFLAGS="-L /usr/lib -L /usr/lib64 -Wl,--script=${with_efi_lds}"

# path to object file from gnu-efi
AC_ARG_WITH([efi-crt0],
            AS_HELP_STRING([--with-efi-crt0=OBJ_PATH],[Path to gnu-efi crt0 object file.]),
            [],
            [with_efi_crt0="/usr/lib/crt0-efi-${ARCH}.o"])
AC_CHECK_FILE(["${with_efi_crt0}"],
              [],
              [AC_MSG_ERROR([Missing ${with_efi_crt0} file.])])
EXTRA_LDLIBS="${with_efi_crt0}"

# check for efi and gnuefi libraries
AC_CHECK_LIB([efi],
             [InitializeLib],
             [EXTRA_LDLIBS="$EXTRA_LDLIBS -lefi"],
             [AC_MSG_WARN([Missing 'InitializeLib' from 'libefi'])])
AC_CHECK_LIB([gnuefi],
             [_relocate],
             [EXTRA_LDLIBS="$EXTRA_LDLIBS -lgnuefi"],
             [AC_MSG_WARN([Missing '_relocate' from 'libgnuefi'])])

# macro to check that compiler will accept a flag, adds flag to EXTRA_CFLAGS
AC_DEFUN(
    [LOCAL_ADD_COMPILER_FLAG],
    [AX_CHECK_COMPILE_FLAG(
        ["$1"],
        [EXTRA_CFLAGS="$EXTRA_CFLAGS $1"],
        [AC_MSG_ERROR([Required CFLAG "$1" not supported by your compiler, aborting.])],
        [-Werror])
    ])
# macro to check that linker will accept a flag, adds flag to EXTRA_LDFLAGS
AC_DEFUN(
    [LOCAL_ADD_LINK_FLAG],
    [AX_CHECK_LINK_FLAG(
        ["$1"],
        [EXTRA_LDFLAGS="$EXTRA_LDFLAGS $1"],
        [AC_MSG_ERROR([Required flag "$1" not supported by your linker.])],
        [-Werror])
    ])
# flags added to EXTRA_CFLAGS
LOCAL_ADD_COMPILER_FLAG([-fno-stack-protector])
LOCAL_ADD_COMPILER_FLAG([-fpic])
LOCAL_ADD_COMPILER_FLAG([-fshort-wchar])
LOCAL_ADD_COMPILER_FLAG([-mno-red-zone])
LOCAL_ADD_COMPILER_FLAG([-Wall])
# flags added to EXTRA_LDFLAGS
LOCAL_ADD_LINK_FLAG([-Wl,-nostdlib])
LOCAL_ADD_LINK_FLAG([-Wl,-znocombreloc])
LOCAL_ADD_LINK_FLAG([-shared])
LOCAL_ADD_LINK_FLAG([-Wl,-Bsymbolic])
# flags added to EXTRA_OBJFLAGS
EXTRA_OBJFLAGS="-j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-${ARCH}"
EXTRA_CFLAGS="$EXTRA_CFLAGS -DARCH=$ARCH"

# export variables to Makefile
AC_SUBST([EXTRA_CFLAGS])
AC_SUBST([EXTRA_LDFLAGS])
AC_SUBST([EXTRA_LDLIBS])
AC_SUBST([EXTRA_OBJFLAGS])
# propagate configure arguments up to distcheck target
AC_SUBST([DISTCHECK_CONFIGURE_FLAGS],[$ac_configure_args])

AC_OUTPUT
