#!/bin/sh

set -u
set -o errexit

CFLAGS="-m64 -Wall -W -pedantic -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -I$topdir/include"

gcc $CFLAGS -std=c99 -c alloy.c -o alloy.o
gcc $CFLAGS -std=gnu99 -c libBareMetal.c -o libBareMetal.o

objcopy --remove-section .comment alloy.o
objcopy --remove-section .eh_frame alloy.o
objcopy --remove-section .comment libBareMetal.o
objcopy --remove-section .eh_frame libBareMetal.o
ld -T app.ld -static -o alloy.bin alloy.o libBareMetal.o "$topdir/lib/libbmfs.a"
