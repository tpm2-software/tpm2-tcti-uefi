#!/usr/bin/env bash

SCANBUILD=${SCANBUILD:-""}
MAKE=${MAKE:-"make -j"}
CC=${CC:-"gcc"}

./bootstrap --include=/usr/share/gnulib/m4
$SCANBUILD ./configure --enable-unit --prefix=${PREFIX} CC=${CC}
$SCANBUILD $MAKE distcheck
$SCANBUILD $MAKE example
