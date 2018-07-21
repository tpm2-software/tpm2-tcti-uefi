# SPDK-License-Identifier: BSD-2
if ARCH_x86_64
    AM_CFLAGS = -DEFI_FUNCTION_WRAPPER
endif

all-local: $(CLEANS)
clean-local:
	rm -rf $(CLEANS)

CLEANS = \
    AUTHORS \
    src/get-capability.efi \
    src/get-capability.o \
    src/get-capability.so \
    src/tcg2-util.o

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

src/get-capability.so: src/get-capability.o src/tcg2-util.o

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS) -c -o $@ $^

%.so: %.o
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ $(EXTRA_LDLIBS)

%.efi: %.so
	$(OBJCOPY) $(OBJFLAGS) $(EXTRA_OBJFLAGS) $^ $@
