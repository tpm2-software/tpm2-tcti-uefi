# SPDK-License-Identifier: BSD-2
#
include tss2-src-vars.mk
AM_CFLAGS = -I$(srcdir)/src -Itpm2-tss/src -Itpm2-tss/include/tss2

TESTS = $(check_PROGRAMS)
lib_LIBRARIES = src/libtss2-mu-uefi.a src/libtss2-sys-uefi.a src/libtss2-tcti-uefi.a
noinst_LIBRARIES = example/libutil.a test/libtest-util.a
check_PROGRAMS = \
    test/tcti-uefi-cast_unit \
    test/tcti-uefi-finalize_unit \
    test/tcti-uefi-init_unit \
    test/tcti-uefi-receive_unit \
    test/tcti-uefi-transmit_unit

all-local: \
    AUTHORS \
    example/get-capability.efi \
    example/tpm2-get-capability.efi
clean-local:
	rm -rf $(CLEANS)
	find tpm2-tss/src -name '*.o' | xargs rm -f

CLEANS = \
    AUTHORS \
    example/get-capability.efi \
    example/get-capability.so \
    src/tcti-send-buf.efi \
    src/tcti-send-buf.so \
    example/tpm2-get-capability.efi \
    example/tpm2-get-capability.so

EXTRA_DIST = \
    AUTHORS \
    CHANGELOG.md \
    CONTRIBUTING.md \
    INSTALL.md \
    LICENSE \
    README.md \
    VERSION

# rules to build libraries
src_libtss2_mu_uefi_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS) -DMAXLOGLEVEL=0
src_libtss2_mu_uefi_a_SOURCES = $(TSS2_MU_C) $(TSS2_UTIL_C)

src_libtss2_sys_uefi_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS) -I$(srcdir)/tpm2-tss/src/tss2-sys/ -DMAXLOGLEVEL=0
src_libtss2_sys_uefi_a_SOURCES = $(TSS2_SYS_C) $(TSS2_UTIL_C)

src_libtss2_tcti_uefi_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS)
src_libtss2_tcti_uefi_a_SOURCES = src/tcg2-util.c src/tcti-uefi.c

example_libutil_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS)
example_libutil_a_SOURCES = example/tss2-util.c example/compat.c

# hack to ensure .deps directory required by autotools is created in the
# 'example' directory before we build any targets
example/.deps:
	mkdir -p $@
example/get-capability.$(OBJEXT): example/get-capability.c | example/.deps
example/tpm2-get-capability.$(OBJEXT): example/tpm2-get-capability.c | example/.deps

# dependency expression for shared objects built for the UEFI executables
example/get-capability.so: CFLAGS+=$(EXTRA_CFLAGS) $(AM_CFLAGS)
example/get-capability.so: example/get-capability.o src/libtss2-tcti-uefi.a

example/tpm2-get-capability.so: CFLAGS+=$(EXTRA_CFLAGS) $(AM_CFLAGS)
example/tpm2-get-capability.so: LDFLAGS+=-Wl,--no-undefined
example/tpm2-get-capability.so: example/tpm2-get-capability.o example/libutil.a \
    src/libtss2-tcti-uefi.a src/libtss2-sys-uefi.a src/libtss2-mu-uefi.a

# rule to extract contributors from git history & generate AUTHORS file
AUTHORS :
	git log --format='%aN <%aE>' | grep -v 'users.noreply.github.com' | sort | \
	    uniq -c | sort -nr | sed 's/^\s*//' | cut -d" " -f2- > $@

# variables to simplify test rules
TEST_FLAGS = $(CMOCKA_CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS)
TEST_LIBS = $(CMOCKA_LIBS) src/libtss2-tcti-uefi.a test/libtest-util.a
TEST_WRAPS = -Wl,--wrap=AllocatePool,--wrap=CopyMem,--wrap=FreePool \
    -Wl,--wrap=Print,--wrap=tcg2_get_capability,--wrap=tcg2_get_max_buf \
    -Wl,--wrap=tcg2_get_protocol,--wrap=tcg2_submit_command

# rule to build test utility library
test_libtest_util_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS)
test_libtest_util_a_SOURCES = test/tcti-uefi-wraps.c

# rules to build test executables
test_tcti_uefi_cast_unit_CFLAGS = $(TEST_FLAGS)
test_tcti_uefi_cast_unit_LDADD = $(TEST_LIBS)
test_tcti_uefi_cast_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_cast_unit_SOURCES = test/tcti-uefi-cast_unit.c

test_tcti_uefi_init_unit_CFLAGS = $(TEST_FLAGS)
test_tcti_uefi_init_unit_LDADD = $(TEST_LIBS)
test_tcti_uefi_init_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_init_unit_SOURCES = test/tcti-uefi-init_unit.c

test_tcti_uefi_receive_unit_CFLAGS = $(TEST_FLAGS)
test_tcti_uefi_receive_unit_LDADD = $(TEST_LIBS)
test_tcti_uefi_receive_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_receive_unit_SOURCES = test/tcti-uefi-receive_unit.c

test_tcti_uefi_transmit_unit_CFLAGS = $(TEST_FLAGS)
test_tcti_uefi_transmit_unit_LDADD = $(TEST_LIBS)
test_tcti_uefi_transmit_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_transmit_unit_SOURCES = test/tcti-uefi-transmit_unit.c

test_tcti_uefi_finalize_unit_CFLAGS = $(TEST_FLAGS)
test_tcti_uefi_finalize_unit_LDADD = $(TEST_LIBS)
test_tcti_uefi_finalize_unit_LDFLAGS = $(TEST_WRAPS)
test_tcti_uefi_finalize_unit_SOURCES = test/tcti-uefi-finalize_unit.c

# pattern rule to link intermediate shared object
%.so: %.o
	$(CC) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ -Bstatic $(EXTRA_LDLIBS)

# pattern to transform intermediate shared object into efi executable
%.efi: %.so
	$(OBJCOPY) $(OBJFLAGS) $(EXTRA_OBJFLAGS) $^ $@
