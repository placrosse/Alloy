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
CFLAGS="-m64 -Wall -Wextra -Werror -Wfatal-errors -g -std=gnu99"
CFLAGS="$CFLAGS -nostdlib -nostartfiles -nodefaultlibs -nostdinc"
CFLAGS="$CFLAGS -fomit-frame-pointer -mno-red-zone"
CFLAGS="$CFLAGS -I$BMFS_INCLUDE_DIR -I$BAREMETAL_LIBC_INCLUDE_DIR"

LD=ld
LDFLAGS="-T alloy.ld -static -L ../../output/lib"

OBJCOPY=objcopy

set -u

$CC $CFLAGS -c alloy.c -o alloy.o
$OBJCOPY --remove-section .comment alloy.o
$OBJCOPY --remove-section .eh_frame alloy.o
$LD $LDFLAGS alloy.o -o alloy.elf -lbmfs -lc
$OBJCOPY -O binary alloy.elf alloy.bin
