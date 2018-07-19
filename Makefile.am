# SPDK-License-Identifier: BSD-2
if ARCH_x86_64
    AM_CFLAGS = -DEFI_FUNCTION_WRAPPER
endif

all-local: AUTHORS src/hello.efi
clean-local:
	rm -rf AUTHORS src/hello.efi

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

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(AM_CFLAGS) -c -o $@ $^

%.so: %.o
	$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) $^ -o $@ $(EXTRA_LDLIBS)

%.efi: %.so
	$(OBJCOPY) $(OBJFLAGS) $(EXTRA_OBJFLAGS) $^ $@
