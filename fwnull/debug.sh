#!/bin/bash
if test -z "$1" || test ! -e "$1"; then
    echo Usage: $0 foo.c
    exit 1
fi

# basic setup
set -x
SL_BUILD=../fwnull_build
SL_PLUG="$SL_BUILD/libfwnull.so"
CC1="../gcc-install/libexec/gcc/`ls ../gcc-install/lib/gcc/`/4.5.0/cc1"

# initial checks
test -x "$CC1"                  || exit 1
test -d "$SL_BUILD"             || exit 1
test -f "$SL_BUILD/Makefile"    || exit 1

# attempt to make
make -C "$SL_BUILD"             || exit 1
test -x "$SL_PLUG"              || exit 1

# use gdb by default
test -z "$GDB" && GDB=gdb

# use verbose level 1 by default
test -z "$SL_VERBOSE" && SL_VERBOSE=1

# use pp code listener by default
test -z "$SL_OPTS" && SL_OPTS="-fplugin-arg-libfwnull-dump-pp"

# run the debugger
"$GDB" --quiet --args "$CC1" "$1" \
    -DPREDATOR \
    -dumpbase test.c -quiet -o /dev/null \
    -m32 -O0 -Wall -Wextra \
    -fplugin="$SL_PLUG" \
    -fplugin-arg-libfwnull-verbose="$SL_VERBOSE" \
    $SL_OPTS
