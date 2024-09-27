# SPDX-License-Identifier: BSD-2
AC_INIT([tpm2-tcti-uefi],
        [0.0.0],
        [https://github.com/tpm2-software/tpm2-tcti-uefi/issues],
        [],
        [https://github.com/tpm2-software/tpm2-tcti-uefi])

AM_INIT_AUTOMAKE([foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])]) #Backward compatible setting of "silent-rules"

AC_CONFIG_FILES([Makefile])

AC_SUBST([DISTCHECK_CONFIGURE_FLAGS],[$ac_configure_args])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AX_IS_RELEASE(dash-version)

AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR([AC_MSG_ERROR([Missing required tool: ar])])

# required programs (beyond compiler / linker)
AC_CHECK_TOOL([OBJCOPY], objcopy, [AC_MSG_ERROR([Missing required tool: objcopy])])

# === Macros ===
AC_DEFUN([CHECK_HEADER_ERROR],
         [AC_CHECK_HEADER([$1],[],[AC_MSG_ERROR([Required header $1 missing])],[$2])])
#AC_CHECK_FILE does not work for cross-compiling
AC_DEFUN([CHECK_FILE],
         [AC_MSG_CHECKING([for $1])
          AS_IF([ test -f "$1" ], AC_MSG_RESULT([yes]),
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([Missing file: $1]) ) ])
# macro to check that compiler will accept a flag, adds flag to EXTRA_CFLAGS
AC_DEFUN([LOCAL_ADD_COMPILER_FLAG],
         [AX_CHECK_COMPILE_FLAG(["$1"],
                                [EXTRA_CFLAGS="$EXTRA_CFLAGS $1"],
                                [AC_MSG_ERROR([Required CFLAG "$1" not supported by your compiler, aborting.])],
                                [-Werror])])
# macro to check that linker will accept a flag, adds flag to EXTRA_LDFLAGS
AC_DEFUN([LOCAL_ADD_LINK_FLAG],
         [AX_CHECK_LINK_FLAG(["$1"],
                             [EXTRA_LDFLAGS="$EXTRA_LDFLAGS $1"],
                             [AC_MSG_ERROR([Required flag "$1" not supported by your linker.])],
                             [-Werror])])


# bring canonical names for build tripple, we need host_cpu
AS_IF([test "x$host_cpu" = "xi386" -o "x$host_cpu" = "xi686"],
      [arch="ia32"
       EXTRA_CFLAGS=-DEFI_FUNCTION_WRAPPER -DARCH_ia32 ],
      [test "x$host_cpu" = "xx86_64"],
      [arch="x86_64"
       EXTRA_CFLAGS=-DARCH_x86_64 ],
      [test "x$host_cpu" = "xaarch64"],
      [arch="aarch64"
       EXTRA_CFLAGS=-DARCH_aarch64 ],
      [AC_MSG_ERROR(["Unsupported host_cpu: $host_cpu"])])
AC_SUBST([ARCH], [$arch])

AX_CODE_COVERAGE
AX_ADD_AM_MACRO_STATIC([])

# unit test library
AC_ARG_ENABLE([unit],
              [AS_HELP_STRING([--enable-unit],
                   [build cmocka unit tests])],
              [], [enable_unit=no])
AM_CONDITIONAL([UNIT], [test "x$enable_unit" = xyes])
AS_IF([test "x$enable_unit" = xyes] && [test "x$host_cpu" = "x$build_cpu"],
      [AC_MSG_ERROR([Unit tests not supported on cross compilation.])])
AS_IF([test "x$enable_unit" = xyes],
      [PKG_CHECK_MODULES([CMOCKA],
                         [cmocka >= 1.0])])

# integration test
AC_ARG_ENABLE([integration],
              [AS_HELP_STRING([--enable-integration],
                  [build and run integration tests])],
              [], [enable_integration=no])
AM_CONDITIONAL([INTEGRATION], [test "x$enable_integration" = xyes])
AS_IF([test "x$enable_integration" = xyes],
      [AC_CHECK_PROG([SWTPM],
                     swtpm,
                     [yes],
                     [AC_MSG_ERROR([Missing required program: swtpm])])
       AC_CHECK_PROG([QEMU],
                     qemu-system-${arch},
                     [yes],
                     [AC_MSG_ERROR([Missing required program: qemu-system-${arch}])])
       AC_ARG_WITH([ovmf],
                    AS_HELP_STRING([--with-ovmf=PATH],
                                   [Path to OVMF.fd]),
                   [],
                   [with_ovmf=/usr/share/qemu/edk2-${arch}-code.fd])
       CHECK_FILE($with_ovmf)
       AC_SUBST([OVMF_PATH],[$with_ovmf])
       ])

# gnu-efi
CHECK_HEADER_ERROR([efi/efi.h],[])
CHECK_HEADER_ERROR([efi/efilib.h], [#include <efi.h>])
CHECK_HEADER_ERROR([efi/${arch}/efibind.h], [#include <efi.h>])

# path to linker script from gnu-efi
AC_ARG_WITH([efi-lds],
            AS_HELP_STRING([--with-efi-lds=LDS_PATH],[Path to gnu-efi lds file.]),
            [],
            [with_efi_lds="/usr/lib/elf_${arch}_efi.lds"])
CHECK_FILE([${with_efi_lds}])
AC_SUBST([EFI_LDS],[$with_efi_lds])


EXTRA_LDFLAGS="-L /usr/lib -L /usr/lib64 -Wl,--script=${with_efi_lds}"

# path to object file from gnu-efi
AC_ARG_WITH([efi-crt0],
            AS_HELP_STRING([--with-efi-crt0=OBJ_PATH],[Path to gnu-efi crt0 object file.]),
            [],
            [with_efi_crt0="/usr/lib/crt0-efi-${arch}.o"])
CHECK_FILE([${with_efi_crt0}])
AC_SUBST([EFI_CRT0],[$with_efi_crt0])

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

# flags added to EXTRA_CFLAGS
LOCAL_ADD_COMPILER_FLAG([-fno-stack-protector])
LOCAL_ADD_COMPILER_FLAG([-fpic])
LOCAL_ADD_COMPILER_FLAG([-fshort-wchar])
#LOCAL_ADD_COMPILER_FLAG([-mno-red-zone])
LOCAL_ADD_COMPILER_FLAG([-Wall])
LOCAL_ADD_COMPILER_FLAG([-Wextra])
# flags added to EXTRA_LDFLAGS
LOCAL_ADD_LINK_FLAG([-Wl,-nostdlib])
LOCAL_ADD_LINK_FLAG([-Wl,-znocombreloc])
LOCAL_ADD_LINK_FLAG([-shared])
LOCAL_ADD_LINK_FLAG([-Wl,-Bsymbolic])
# flags added to EXTRA_OBJFLAGS
EXTRA_OBJFLAGS="-j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-${arch} --subsystem=10"

# export variables to Makefile
AC_SUBST([EXTRA_CFLAGS])
AC_SUBST([EXTRA_LDFLAGS])
AC_SUBST([EXTRA_LDLIBS])
AC_SUBST([EXTRA_OBJFLAGS])
# propagate configure arguments up to distcheck target
AC_SUBST([DISTCHECK_CONFIGURE_FLAGS],[$ac_configure_args])

AC_OUTPUT

AC_MSG_RESULT([
    $PACKAGE_NAME $VERSION

    architecture:      $arch
    unit tests:        $enable_unit
    integration tests: $enable_integration
])