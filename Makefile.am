# SPDK-License-Identifier: BSD-2
#
.PHONY: example
AM_CFLAGS = -I$(srcdir)/src

TESTS = $(check_PROGRAMS)
lib_LIBRARIES = src/libtss2-tcti-uefi.a
noinst_LIBRARIES = test/libtest-util.a
if UNIT
check_PROGRAMS = \
    test/tcti-uefi-cast_unit \
    test/tcti-uefi-finalize_unit \
    test/tcti-uefi-init_unit \
    test/tcti-uefi-receive_unit \
    test/tcti-uefi-transmit_unit
endif

tss2dir = $(includedir)/tss2
tss2_HEADERS = src/tss2-tcti-uefi.h

all-local: AUTHORS

example: \
    example/hello-world.efi \
    example/tcg2-get-caps.efi \
    example/tpm2-get-caps-fixed.efi

clean-local:
	rm -rf $(CLEANS)

CLEANS = \
    AUTHORS \
    example/hello-world.efi \
    example/hello-world.so \
    example/tcg2-get-caps.efi \
    example/tcg2-get-caps.so \
    example/tpm2-get-caps-fixed.efi \
    example/tpm2-get-caps-fixed.so \
    example/*.$(OBJEXT)

EXTRA_DIST = \
    AUTHORS \
    CHANGELOG.md \
    CONTRIBUTING.md \
    INSTALL.md \
    LICENSE \
    README.md \
    VERSION

# rules to build libraries
src_libtss2_tcti_uefi_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS)
src_libtss2_tcti_uefi_a_SOURCES = src/tcti-uefi.c src/tcti-uefi.h \
    src/tcg2-protocol.h src/uefi-types.h src/tcg2-util.c src/tcg2-util.h

# hack to ensure .deps directory required by autotools is created in the
# 'example' directory before we build any targets
example/.deps:
	mkdir -p $@
example/hello-world.$(OBJEXT): example/hello-world.c | example/.deps
example/tcg2-get-caps.$(OBJEXT): example/tcg2-get-caps.c \
    src/tcg2-protocol.h src/tcg2-util.h | example/.deps
example/tpm2-get-caps-fixed.$(OBJEXT): example/tpm2-get-caps-fixed.c \
    example/compat.h example/tss2-util.h | example/.deps

# dependency expression for shared objects built for the UEFI executables
example/tcg2-get-caps.so: CFLAGS+=$(EXTRA_CFLAGS) $(AM_CFLAGS)
example/tcg2-get-caps.so: LDFLAGS+=-Wl,--no-undefined
example/tcg2-get-caps.so: example/tcg2-get-caps.o src/tcg2-util.o

example/tpm2-get-caps-fixed.so: CFLAGS+=$(EXTRA_CFLAGS) $(AM_CFLAGS)
example/tpm2-get-caps-fixed.so: LDFLAGS+=-Wl,--no-undefined
example/tpm2-get-caps-fixed.so: LDLIBS+=-l:libtss2-sys.a -l:libtss2-mu.a
example/tpm2-get-caps-fixed.so: \
    example/tpm2-get-caps-fixed.o example/tss2-util.o example/compat.o \
    src/libtss2-tcti-uefi.a

example/hello-world.so: CFLAGS+=$(EXTRA_CFLAGS) $(AM_CFLAGS)
example/hello-world.so: LDFLAGS+=-Wl,--no-undefined
example/hello-world.so: example/hello-world.$(OBJEXT)

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

if UNIT
# rule to build test utility library
test_libtest_util_a_CFLAGS = $(EXTRA_CFLAGS) $(AM_CFLAGS)
test_libtest_util_a_SOURCES = test/tcti-uefi-wraps.c test/tcti-uefi-wraps.h

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
endif

# pattern rule to link intermediate shared object
%.so: %.o
	$(CC) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ -Bstatic $(LDLIBS) $(EXTRA_LDLIBS)

# pattern to transform intermediate shared object into efi executable
%.efi: %.so
	$(OBJCOPY) $(OBJFLAGS) $(EXTRA_OBJFLAGS) $^ $@
