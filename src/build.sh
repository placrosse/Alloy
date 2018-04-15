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

$CC $CFLAGS -c alloy.c

if [ -z ${ALLOY_WITH_BAREMETAL+x} ]; then
	$CC $CFLAGS -c xterm.c
	$CC $CFLAGS -c host-linux.c
	$LD $LDFLAGS -o alloy xterm.o host-linux.o alloy.o ../lib/liballoy.a
else
	CFLAGS="$CFLAGS -nostdinc"
	CFLAGS="$CFLAGS -DALLOY_WITH_BAREMETAL=1"
	LDFLAGS="$LDFLAGS -nostdlib -nostartfiles -nodefaultlibs -T alloy.ld"
	$CC $CFLAGS -c font.c
	$CC $CFLAGS -c vesaterm.c
	$CC $CFLAGS -c host-baremetal.c
	$LD $LDFLAGS -o alloy alloy.o font.o vesaterm.o host-baremetal.o ../lib/liballoy.a
	$OBJCOPY -O binary alloy alloy.bin
fi

