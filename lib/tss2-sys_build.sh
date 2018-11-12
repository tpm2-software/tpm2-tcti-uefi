#!/usr/bin/env sh

usage_error ()
{
    echo "$0: $*" >&2
    print_usage >&2
    exit 2
}
print_warning ()
{
    echo "$0 WARNING: $*" >&2
}
print_error ()
{
    echo "$0 ERROR: $*" >&2
    exit 1
}
print_usage ()
{
    cat <<END
Usage:
    tpm2-tss_build --src=[path] --build=[path] --dest=[path]
END
}
check_dir ()
{
    if [ ! -d $1 ]; then exit 1; fi
}
while test $# -gt 0; do
    case $1 in
    --help) print_usage; exit $?;;
    -s=*|--src=*) SRC_DIR="${1#*=}"; check_dir $SRC_DIR;;
    -b=*|--build=*) BUILD_DIR="${1#*=}"; check_dir $BUILD_DIR;;
    -d=*|--dest=*) DEST_DIR="${1#*=}"; check_dir $DEST_DIR;;
    --) shift; break;;
    -*) usage_error "invalid option: '$1'";;
     *) break;;
    esac
    shift
done
CONFIGURE=$SRC_DIR/configure
if [ ! -x "$CONFIGURE" ]; then
    print_error "configure script not executable: not bootstrapped?"
fi
if [ -z "$CONFIG_SITE" ]; then
    print_warning "CONFIG_SITE not set, this could get weird"
elif [ ! -f "$CONFIG_SITE" ]; then
    print_error "CONFIG_SITE set but not a real file: $CONFIG_SITE"
fi

cd $BUILD_DIR
$CONFIGURE --disable-defaultflags --disable-shared --with-maxloglevel=none --disable-tcti-device --disable-tcti-mssim --disable-esapi --disable-doxygen-doc
DESTDIR=$DEST_DIR make --jobs=$(($(nproc)*3/2)) install
cd -
