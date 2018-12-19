FROM debian:buster AS builder

ARG PREFIX=/usr
ARG MAKE="make -j"

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
  apt-get -y install \
    libcmocka-dev \
    libcmocka0 \
    gnulib \
    gnu-efi \
    automake \
    autoconf \
    libtool \
    gcc \
    clang \
    clang-tools \
    build-essential \
    libssl-dev \
    dh-exec \
    pkg-config \
    libfuse-dev \
    libglib2.0-dev \
    libgmp-dev \
    expect \
    libtasn1-dev \
    socat \
    tpm-tools \
    python3-twisted \
    gnutls-dev \
    gnutls-bin \
    wget \
    qemu \
    qemu-system-x86 \
    ovmf \
    git \
    net-tools \
    gawk

WORKDIR /tmp/deps
RUN export retdir=$PWD && \
  export tmpdir=$(mktemp -d) && \
  cd $tmpdir && \
  wget https://download.01.org/tpm2/autoconf-archive-2017.09.28.tar.xz && \
  sha256sum autoconf-archive-2017.09.28.tar.xz | \
    grep -q 5c9fb5845b38b28982a3ef12836f76b35f46799ef4a2e46b48e2bd3c6182fa01 && \
  tar xJf autoconf-archive-2017.09.28.tar.xz && \
  cd autoconf-archive-2017.09.28 && \
  ./configure --prefix=${PREFIX} && \
  $MAKE && \
  $MAKE install && \
  cd .. && \
  git clone -b master --single-branch \
    https://github.com/tpm2-software/tpm2-tss.git && \
  cd tpm2-tss && \
  ./bootstrap --include=/usr/share/gnulib/m4 && \
  CONFIG_SITE=../lib/tss2-sys_config.site ./configure --prefix=${PREFIX} \
    --disable-defaultflags --disable-doxygen-doc --disable-tcti-device \
    --disable-tcti-mssim --disable-esapi --with-maxloglevel=none && \
  $MAKE && \
  $MAKE install && \
  cd .. && \
  git clone -b master --single-branch \
    https://github.com/stefanberger/libtpms && \
  cd libtpms && \
  ./autogen.sh --with-openssl --prefix=${PREFIX} --with-tpm2 && \
  $MAKE && \
  $MAKE install && \
  cd .. && \
  git clone -b master --single-branch https://github.com/stefanberger/swtpm && \
  cd swtpm && \
  ./autogen.sh --with-openssl --prefix=${PREFIX} CC=gcc && \
  $MAKE && \
  $MAKE install && \
  cd .. && \
  cd $retdir && \
  rm -rf $tmpdir

WORKDIR /src/tpm2-tcti-uefi


FROM builder

ARG CC=clang
ARG SCANBUILD="scan-build --status-bugs"

COPY . /src/tpm2-tcti-uefi
RUN ./build.sh
