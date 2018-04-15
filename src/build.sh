#!/bin/bash

set -e

../utils/alloy-font-generator

LD=gcc
LDFLAGS=

CC=gcc
CFLAGS="$CFLAGS -Wall -Wextra -Werror -Wfatal-errors -g"
CFLAGS="$CFLAGS -fomit-frame-pointer -fno-stack-protector -mno-red-zone"
CFLAGS="$CFLAGS -I ../include"
CFLAGS="$CFLAGS -std=gnu99"

OBJCOPY=objcopy
NASM=nasm

if [ -z ${ALLOY_WITH_BAREMETAL+x} ]; then
	CFLAGS="$CFLAGS -nostdlib -nostartfiles -nodefaultlibs -nostdinc"
	LDFLAGS="$LDFLAGS -nostdlib -nostartfiles -nodefaultlibs -T alloy.ld"
fi

$CC $CFLAGS -c alloy.c

if [ -z ${ALLOY_WITH_BAREMETAL+x} ]; then
	$CC $CFLAGS -c xterm.c
	$CC $CFLAGS -c host-linux.c
	$LD $LDFLAGS -o alloy xterm.o host-linux.o alloy.o ../lib/liballoy.a
else
	$CC $CFLAGS -c font.c
	$CC $CFLAGS -c vesaterm.c
	$CC $CFLAGS -c host-baremetal.c
	$LD $LDFLAGS -o alloy font.o vesaterm.o host-baremetal.o alloy.o ../lib/liballoy.a
	$OBJCOPY -O binary alloy alloy.bin
	$NASM loader.asm -o loader.bin
fi


$LD $LDFLAGS alloy.o xterm.o host-linux.o ../lib/liballoy.a -o alloy
