# Testing
Tests in this repo are currently limited to unit tests. These tests must be
explicitly enabled at configure time. Use the `--help` option to the
`configure`script for documentation. If enabled, unit tests are integrated
into the autotools build using the automake test harness. They are built
and executed using the `check` make target:
> make check
