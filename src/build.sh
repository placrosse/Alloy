#!/bin/bash

set -e
set -u

../utils/alloy-font-generator

TOP=../../..

CC=gcc
CFLAGS="-m64 -Wall -Wextra -Werror -Wfatal-errors -g -std=gnu99"
CFLAGS="$CFLAGS -nostdlib -nostartfiles -nodefaultlibs -nostdinc"
CFLAGS="$CFLAGS -fomit-frame-pointer -fno-stack-protector -mno-red-zone"
CFLAGS="$CFLAGS -I $TOP/output/include -I ../include"

LD=ld
LDLIBS="-lc -lbmfs"
LDFLAGS="-static -L $TOP/output/lib -T alloy.ld"

OBJCOPY=objcopy

$CC $CFLAGS -c alloy.c -o alloy.o
$OBJCOPY --remove-section .comment alloy.o
$OBJCOPY --remove-section .eh_frame alloy.o

$CC $CFLAGS -c font.c -o font.o
$OBJCOPY --remove-section .comment font.o
$OBJCOPY --remove-section .eh_frame font.o

$CC $CFLAGS -c term.c -o term.o
$CC $CFLAGS -c vesaterm.c -o vesaterm.o

$LD $LDFLAGS alloy.o font.o term.o vesaterm.o -o alloy $LDLIBS

$OBJCOPY -O binary alloy alloy.bin
