#!/bin/bash

set -e
set -u

../utils/alloy-font-generator

TOP=../../..

CC=gcc
CFLAGS="-m64 -Wall -Wextra -Werror -Wfatal-errors -g -std=gnu99"
CFLAGS="$CFLAGS -nostdlib -nostartfiles -nodefaultlibs -nostdinc"
CFLAGS="$CFLAGS -fomit-frame-pointer -fno-stack-protector -mno-red-zone"
CFLAGS="$CFLAGS -I ../include -I $TOP/output/include"

if ! test -z "$ALLOY_WITH_BAREMETAL"; then
	CFLAGS="$CFLAGS -DALLOY_WITH_BAREMETAL=1"
fi

LD=ld
LDLIBS="-lc -lbmfs"
LDFLAGS="-static -L $TOP/output/lib -T alloy.ld"

OBJCOPY=objcopy

NASM=nasm

$NASM loader.asm -o loader.bin

$CC $CFLAGS -c alloy.c -o alloy.o
$CC $CFLAGS -c font.c -o font.o
$CC $CFLAGS -c input.c -o input.o
$CC $CFLAGS -c shell.c -o shell.o
$CC $CFLAGS -c term.c -o term.o
$CC $CFLAGS -c vesaterm.c -o vesaterm.o
$NASM syscall-hooks.asm -f elf64 -g -F dwarf -o syscall-hooks.o

$LD $LDFLAGS alloy.o font.o input.o shell.o syscall-hooks.o term.o vesaterm.o -o alloy.elf $LDLIBS

$OBJCOPY -O binary alloy.elf alloy.bin
