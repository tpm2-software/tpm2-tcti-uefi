# SPDK-License-Identifier: BSD-2
AM_CFLAGS = -I $(srcdir)/src
if ARCH_x86_64
    AM_CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

TESTS = $(check_PROGRAMS)
check_PROGRAMS = \
    test/tcti-uefi-init_unit

all-local: \
    AUTHORS \
    src/get-capability.efi \
    src/libtss2-tcti-uefi.a \
    src/tcti-init.efi \
    src/tcti-send-buf.efi
clean-local:
	rm -rf $(CLEANS)

CLEANS = \
    AUTHORS \
    src/get-capability.efi \
    src/get-capability.o \
    src/get-capability.so \
    src/libtss2-tcti-uefi.a \
    src/tcg2-util.o \
    src/tcti-uefi.o \
    src/tcti-init.efi \
    src/tcti-init.o \
    src/tcti-init.so \
    src/tcti-send-buf.efi \
    src/tcti-send-buf.o \
    src/tcti-send-buf.so

EXTRA_DIST = \
    AUTHORS \
    CHANGELOG.md \
    CONTRIBUTING.md \
    INSTALL.md \
    LICENSE \
    README.md \
    VERSION

AUTHORS :
	git log --format='%aN <%aE>' | grep -v 'users.noreply.github.com' | sort | \
	    uniq -c | sort -nr | sed 's/^\s*//' | cut -d" " -f2- > $@

src/libtss2-tcti-uefi.a: src/tcg2-util.o src/tcti-uefi.o
	$(AR) -cr $@ $?

src/get-capability.so: src/get-capability.o src/libtss2-tcti-uefi.a

src/tcti-init.so: src/tcti-init.o src/libtss2-tcti-uefi.a

src/tcti-send-buf.so: src/tcti-send-buf.o src/libtss2-tcti-uefi.a

test_tcti_uefi_init_unit_CFLAGS = $(CMOCKA_CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS)
test_tcti_uefi_init_unit_LDADD = $(CMOCKA_LIBS) src/tcti-uefi.o
test_tcti_uefi_init_unit_LDFLAGS = -Wl,--wrap=AllocatePool,--wrap=CopyMem,--wrap=FreePool,--wrap=LibLocateProtocol,--wrap=Print,--wrap=tcg2_get_capability,--wrap=tcg2_get_max_buf,--wrap=tcg2_submit_command
test_tcti_uefi_init_unit_SOURCES = test/tcti-uefi-init_unit.c

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS) -c -o $@ $^

%.so: %.o src/libtss2-tcti-uefi.a
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ -Bstatic $(EXTRA_LDLIBS)

%.efi: %.so
	$(OBJCOPY) $(OBJFLAGS) $(EXTRA_OBJFLAGS) $^ $@
