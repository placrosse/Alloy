#!/bin/bash

set -e

cd fonts/source-code-pro
./build.sh
cd ../..

CC=gcc
CFLAGS="-m64 -Wall -Wextra -Werror -Wfatal-errors -g -std=gnu99"

$CC $CFLAGS alloy-font-generator.c -o alloy-font-generator `pkg-config --cflags --libs freetype2`

./alloy-font-generator

CFLAGS="$CFLAGS -nostdlib -nostartfiles -nodefaultlibs -nostdinc"
CFLAGS="$CFLAGS -fomit-frame-pointer -fno-stack-protector -mno-red-zone"
CFLAGS="$CFLAGS -I$BMFS_INCLUDE_DIR -I$BAREMETAL_LIBC_INCLUDE_DIR"

LD=ld
LDFLAGS="-T alloy.ld -static -L ../../output/lib"

OBJCOPY=objcopy

set -u

$CC $CFLAGS -c alloy.c -o alloy.o
$OBJCOPY --remove-section .comment alloy.o
$OBJCOPY --remove-section .eh_frame alloy.o

$CC $CFLAGS -c alloy-font.c -o alloy-font.o
$OBJCOPY --remove-section .comment alloy-font.o
$OBJCOPY --remove-section .eh_frame alloy-font.o

#$LD $LDFLAGS alloy.o -o alloy.elf -lbmfs -lc

$LD $LDFLAGS alloy.o alloy-font.o -o alloy.elf -lbmfs -lc

$OBJCOPY -O binary alloy.elf alloy.bin
