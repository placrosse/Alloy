#!/bin/bash

set -e

if [ "$BMFS_INCLUDE_DIR" == "" ]; then
	echo "BMFS include directory not set."
	exit 1
fi

if [ "$BMFS_LIBRARY" == "" ]; then
	echo "BMFS library not set."
	exit 1
fi

if [ "$BAREMETAL_LIBC_INCLUDE_DIR" == "" ]; then
	echo "BareMetal libc include directory not set."
	exit 1
fi

if [ "$BAREMETAL_LIBC_LIBRARY" == "" ]; then
	echo "BareMetal libc library not set."
	exit 1
fi

CC=gcc
CFLAGS="-m64 -Wall -Wextra -Werror -Wfatal-errors"
CFLAGS="$CFLAGS -nostdlib -nostartfiles -nodefaultlibs -nostdinc"
CFLAGS="$CFLAGS -fomit-frame-pointer -mno-red-zone"
CFLAGS="$CFLAGS -I$BMFS_INCLUDE_DIR -I$BAREMETAL_LIBC_INCLUDE_DIR"

LD=ld
LDFLAGS="-T app.ld -static"

set -u

function compile {
	echo "CC $1"
	$CC $CFLAGS -c $1 -o $2
	objcopy --remove-section .comment $2
	objcopy --remove-section .eh_frame $2
}

compile alloy.c alloy.o

function link {
	echo "LD $1"
	$LD $LDFLAGS -o $1 ${@:2}
}

link alloy.bin alloy.o $BMFS_LIBRARY $BAREMETAL_LIBC_LIBRARY
