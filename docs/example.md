# Example
The source files in the `example/` directory are provided as references
to demonstrate use of the TPM2 APIs available under UEFI. These
applications are built using gnu-efi and are intended for use as
executables launched from the UEFI shell (indicated by the `.efi`
suffix in the file name).

## Dependencies
Before the example applications can be built you must build `libtss2-sys`
and `libtss2-mu` for use in UEFI. The source code for these two libraries
may be obtained from [tpm2-tss](https://github.com/tpm2-software/tpm2-tss).
Building these libraries for use in UEFI is beyond the scope of this
document. For the project CI we do so using the ELF Linux toolchain and a
custom build environment / CONFIG_SITE file. These files are available in
the $(srcdir)/.ci directory from git. These files are not shipped in source
distributions as they are not a supported part of the project.

A good example of building UEFI executables and libraries on Linux is
here: https://www.rodsbooks.com/efi-programming/index.html
The CONFIG_SITE file used by our CI loop is here:
[tss2-sys_config.site](lib/tss2-sys_config.site).

The details of `config.site` files are beyond the scope of this document.
The reader is expected to be familiar with this mechanism and should
consult the GNU documentation for details:
https://www.gnu.org/software/automake/manual/html_node/config_002esite.html

Once these dependencies have been satisfied the example applications may
be built by executing the `example` make target:
> make example

## Query TCG EFI Protocol for capabilities
The `tcg2-get-caps.efi` example UEFI executable demonstrates use of the
TCG2 UEFI protocol. Looking at the UEFI environment as an operating
system the TCG2 UEFI protocol is analogous to an OS driver. It presents
us with a very primitive interface to the TPM2 device with a design that
allows us to build more feature rich interfaces on top of it. The TCG2
UEFI Protocol driver is documented in the relevant TCG specification:
https://trustedcomputinggroup.org/wp-content/uploads/EFI-Protocol-Specification-rev13-160330final.pdf

This application demonstrates use of the `EFI_TCG2_GET_CAPABILITY`
function by querying the driver for its capabilities / state and then
displaying this data UEFI shell output. NOTE: the data returned by this
function is different from the `TPM2_GetCapability` command from the TPM2
specification.

## Get TPM2 Fixed Properties with libtss2-sys
The `tpm2-get-caps-fixed.efi` example UEFI executable demonstrates use of
the TCG system API (aka `libtss2-sys`). In order to use `libtss2-sys` in
this UEFI application we must have an instance of the UEFI TCTI from
`libtss2-tcti-uefi`. An instance of this TCTI must be passed to the
system API context when initialized.

Once the system API instance is initialized this application uses it to
send the `TPM2_GetCapability` command using the `Tss2_Sys_GetCapability`
function. It queries the TPM2 device for it's fixed properties by
specifying the `TPM2_CAP_TPM_PROPERTIES` capability the `TPM2_PT_FIXED`
property. This collection of properties is then displayed as UEFI shell
output.

## Running the example applications
The example applications may be run from the UEFI shell on your computer.
A test setup like this is often referred to as a "bare-metal" test
environment. Alternatively they may be run in a simulated / virtual
firmware environment using QEMU, OVMF and the swtpm daemon.

Each configuration has its pros / cons. Primarily testing on "bare-metal"
requires less setup and configuration but requires additional hardware or
disruption to development caused by rebooting. A virtual test environment
takes much longer to configure due to the additional software requirements
however no extra hardware is needed and test cycles are very fast since no
reboots are required.

### bare metail
Running the example applications on a "bare metal" system (PC / laptop)
requires the ability to access the UEFI shell. The method and interface
that allows access to UEFI and the shell is not standardized across
platforms. It is common for firmware configuration interfaces to provide a
graphical / menu interface that allows users to boot directly to the
shell. Other less feature ritch platforms will provide an interface that
allows users to create custom boot configurations that may be used to boot
a UEFI application directly and this may be used to boot the shell.

Given the variability in platforms disucssed above the specific method
used to launch the shell is outside the scope of this document. Instead
we recommend the reader consult the Intel UEFI shell documentation:
https://software.intel.com/en-us/articles/uefi-shell

Once you're able to boot to the UEFI shell, the `.efi` executables for
the example UEFI applications must be built and copied to a FAT partition
accessible from the shell. They can then be executed like any other UEFI
application.

### QEMU, OVMF and swtpm
An alternative method we can use to run these example applications is to
use emulation instead of a physical system. Though there are likely many
different ways to achieve this, we limit our discussion to QEMU, OVMF and
swtpm. Typically the QEMU and OVMF components can be installed using the
package management system in your Linux distro. Both versions currently
shipping with Debian Buster (2.12.1 and 20181115 respectively) support all
of the features we require.

The `swtpm` software must currently be built from source. You should
follow the build and installation instructions from the swtpm source
repo: https://github.com/stefanberger/swtpm and their wiki:
https://github.com/stefanberger/swtpm/wiki.

Once all dependencies are satisfied we start by creating a `swtpm`
instance. It should be configured to be a TPM2 device and to expose a
socket interface for use by QEMU. For example use of `swtpm` in the
automated test harness see
[lib/efi-test-setup.sh](lib/efi-test-setup.sh).

Once the `swtpm` instance is running we run `qemu` for the `x86_64`
architecture using the OVMF image installed by the package manager and
configured to use our `swtpm` instance as the TPM2 device. The details of
each QEMU command line option used is beyond the scope of this document.
For example us of `qemu` with `swtpm` see
[lib/efi-test-setup.sh](lib/efi-test-setup.sh).
