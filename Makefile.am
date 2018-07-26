# SPDK-License-Identifier: BSD-2
if ARCH_x86_64
    AM_CFLAGS = -DEFI_FUNCTION_WRAPPER
endif

all-local: \
    AUTHORS \
    src/get-capability.efi \
    src/libtss2-tcti-uefi.a

clean-local:
	rm -rf $(CLEANS)

CLEANS = \
    AUTHORS \
    src/get-capability.efi \
    src/get-capability.o \
    src/get-capability.so \
    src/libtss2-tcti-uefi.a \
    src/tcg2-util.o \
    src/tcti-uefi.o

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

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS) -c -o $@ $^

%.so: %.o src/libtss2-tcti-uefi.a
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ -Bstatic $(EXTRA_LDLIBS)

%.efi: %.so
	$(OBJCOPY) $(OBJFLAGS) $(EXTRA_OBJFLAGS) $^ $@
