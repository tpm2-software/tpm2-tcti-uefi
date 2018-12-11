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
The default flags passed by the tpm2-tss build to the compiler and linker
will *not* work properly in UEFI. To build these libraries for use in UEFI
we provide compatible flags in an automake `config.site` file:
[tss2-sys_config.site](lib/tss2-sys_config.site).

The details of `config.site` files are beyond the scope of this document.
The reader is expected to be familiar with this mechanism and should
consult the GNU documentation for details:
https://www.gnu.org/software/automake/manual/html_node/config_002esite.html

An example use of the `tss2-sys_config.site` file can be found in
[.travis-ci.yml](.travis-ci.yml) configuration file. We must build these
libraries as part of building the example applications in the CI loop.

Once these dependencies have been satisfied the example applications may
be built by executing the `example` make target:
> make example

## Query TCG EFI Protocol for capabilities
The `get-capability.efi` example UEFI executable demonstrates use of the
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
The `tpm2-get-capability.efi` example UEFI executable demonstrates use of
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
