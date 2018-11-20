This file contains instructions for building and installing tss2-tcti-uefi.

# Dependencies
* GNU Autoconf
* GNU Automake
* C compiler
* linker
* gnu-efi (>= 3.0.8)
* tpm2-tss

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
