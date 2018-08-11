# SPDK-License-Identifier: BSD-2
#
include tss2-src-vars.mk
AM_CFLAGS = -I $(srcdir)/src -Itpm2-tss/src -Itpm2-tss/include/tss2

TESTS = $(check_PROGRAMS)
lib_LIBRARIES = \
    src/libtss2-mu-uefi.a \
    src/libtss2-tcti-uefi.a
check_PROGRAMS = \
    test/tcti-uefi-cast_unit \
    test/tcti-uefi-finalize_unit \
    test/tcti-uefi-init_unit \
    test/tcti-uefi-receive_unit \
    test/tcti-uefi-transmit_unit

all-local: \
    AUTHORS \
    src/get-capability.efi \
    src/tcti-send-buf.efi
clean-local:
	rm -rf $(CLEANS)
	find tpm2-tss/src -name '*.o' | xargs rm -f

CLEANS = \
    AUTHORS \
    src/get-capability.efi \
    src/get-capability.o \
    src/get-capability.so \
    src/libtss2-tcti-uefi.a \
    src/libtss2-mu-uefi.a \
    src/tcg2-util.o \
    src/tcti-uefi.o \
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

src_libtss2_mu_uefi_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS) -DMAXLOGLEVEL=0
src_libtss2_mu_uefi_a_SOURCES = $(TSS2_MU_SRC)

src_libtss2_tcti_uefi_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS)
src_libtss2_tcti_uefi_a_SOURCES = src/tcg2-util.c src/tcti-uefi.c

src/get-capability.so: src/get-capability.o src/libtss2-tcti-uefi.a

src/tcti-send-buf.so: src/tcti-send-buf.o src/libtss2-tcti-uefi.a src/libtss2-mu-uefi.a

TEST_WRAPS = -Wl,--wrap=AllocatePool,--wrap=CopyMem,--wrap=FreePool \
    -Wl,--wrap=Print,--wrap=tcg2_get_capability,--wrap=tcg2_get_max_buf \
    -Wl,--wrap=tcg2_get_protocol,--wrap=tcg2_submit_command

test_tcti_uefi_cast_unit_CFLAGS = $(CMOCKA_CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS)
test_tcti_uefi_cast_unit_LDADD = $(CMOCKA_LIBS) src/tcti-uefi.o test/tcti-uefi-wraps.o
test_tcti_uefi_cast_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_cast_unit_SOURCES = test/tcti-uefi-cast_unit.c

test_tcti_uefi_init_unit_CFLAGS = $(CMOCKA_CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS)
test_tcti_uefi_init_unit_LDADD = $(CMOCKA_LIBS) src/tcti-uefi.o test/tcti-uefi-wraps.o
test_tcti_uefi_init_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_init_unit_SOURCES = test/tcti-uefi-init_unit.c

test_tcti_uefi_receive_unit_CFLAGS = $(CMOCKA_CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS)
test_tcti_uefi_receive_unit_LDADD = $(CMOCKA_LIBS) src/tcti-uefi.o test/tcti-uefi-wraps.o
test_tcti_uefi_receive_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_receive_unit_SOURCES = test/tcti-uefi-receive_unit.c

test_tcti_uefi_transmit_unit_CFLAGS = $(CMOCKA_CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS)
test_tcti_uefi_transmit_unit_LDADD = $(CMOCKA_LIBS) src/tcti-uefi.o test/tcti-uefi-wraps.o
test_tcti_uefi_transmit_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_transmit_unit_SOURCES = test/tcti-uefi-transmit_unit.c

test_tcti_uefi_finalize_unit_CFLAGS = $(CMOCKA_CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS)
test_tcti_uefi_finalize_unit_LDADD = $(CMOCKA_LIBS) src/tcti-uefi.o test/tcti-uefi-wraps.o
test_tcti_uefi_finalize_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_finalize_unit_SOURCES = test/tcti-uefi-finalize_unit.c

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS) -c -o $@ $^

%.so: %.o
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ -Bstatic $(EXTRA_LDLIBS)

%.efi: %.so
	$(OBJCOPY) $(OBJFLAGS) $(EXTRA_OBJFLAGS) $^ $@
