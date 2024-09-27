This file contains instructions for building and installing tss2-tcti-uefi.

Also see .ci/build-deps.sh for setting up dependencies

# Dependencies
* GNU Autoconf
* GNU Automake
* C compiler
* linker
* gnu-efi (>= 3.0.18)
* tpm2-tss for tests and examples
* qemu (>= 9.1.0) for integration tests

# Build Instructions
This project makes an attempt to follow the GNU Makefile conventions closely
and aims to be a "well behaved" autotools project. This allows users to
augment and override the environment in a mostly standard way. For
information on this convention refer to the GNU standard:
https://www.gnu.org/prep/standards/html_node/Makefile-Conventions.html

When building from git the `bootstrap` script must be run as the first step.
If you need to override the autoconf / aclocal environment (`ACLOCAL_PATH`
etc) you will need to do so before running `bootstrap`:
> ./bootstrap

The source code is then configured by executing the `configure` script:
> ./configure

The UEFI TCTI library (libtss2-tcti-uefi.a) is then built by the default
make target:
> make

Finally `libtss2-tcti-uefi.a` and the associated header file are installed by
executing the `install` make target:
> make install

# UEFI on ARM

Setup the prefix for the ARM libraries:
```
export PREFIX=$PWD/../INSTALL
mkdir -p $PREFIX
```

tpm2-tss:
```
./bootstrap
CONFIG_SITE=tss2-sys_config.site ./configure --prefix=$PREFIX --build x86_64-pc-linux-gnu --host aarch64-linux-gnu --disable-defaultflags --disable-doxygen-doc --disable-tcti-device --disable-tcti-mssim --disable-tcti-spidev --disable-esys --disable-fapi --disable-policy --with-maxloglevel=none
make -j
make install
```

gnu-efi:
```
make CROSS_COMPILE=aarch64-linux-gnu- PREFIX=$PREFIX
make install CROSS_COMPILE=aarch64-linux-gnu- PREFIX=$PREFIX
```

swtpm:
```
apt install swtpm
```

qemu:
https://wiki.qemu.org/Hosts/Linux
```
./configure --prefix=$PREFIX --enable-tpm --disable-kvm --targets=aarch64
make -j
make install
```

tpm2-tcti-uefi:
```
./bootstrap
./configure --build x86_64-pc-linux-gnu --host aarch64-linux-gnu --enable-integration \
    --with-efi-lds=$PREFIX/lib/elf_aarch64_efi.lds --with-efi-crt0=$PREFIX/lib/crt0-efi-aarch64.o \
    --with-efi-includedir=$PREFIX/include --with-ovmf=$PREFIX/qemu-9.1.0/INSTALL/share/qemu/edk2-aarch64-code.fd \
    --prefix=$PREFIX CPPFLAGS="-I$PREFIX/include -I$PREFIX/include/efi -I$PREFIX/include/efi/aarch64" LDFLAGS=-L$PREFIX/lib
make -j
make check
```