#!/bin/sh

set -e

CC=gcc
CFLAGS="${CFLAGS} -Wall -Wextra -Werror -Wfatal-errors"
CFLAGS="${CFLAGS} -fno-stack-protector"
CFLAGS="${CFLAGS} -fomit-frame-pointer"
CFLAGS="${CFLAGS} -mno-red-zone"
CFLAGS="${CFLAGS} -I ../include"
CFLAGS="${CFLAGS} -std=gnu99"
CFLAGS="${CFLAGS} -g"

$CC $CFLAGS -c app.c
$CC $CFLAGS -c app-bin.c
$CC $CFLAGS -c app-elf.c
$CC $CFLAGS -c cmd.c
$CC $CFLAGS -c color.c
$CC $CFLAGS -c heap.c
$CC $CFLAGS -c host.c
$CC $CFLAGS -c input.c
$CC $CFLAGS -c scanner.c
$CC $CFLAGS -c scheme.c
$CC $CFLAGS -c shell.c
$CC $CFLAGS -c string.c
$CC $CFLAGS -c term.c
$CC $CFLAGS -c var.c

AR=ar
ARFLAGS=rcs

$AR $ARFLAGS liballoy.a app.o app-bin.o app-elf.o cmd.o color.o heap.o host.o input.o scanner.o scheme.o shell.o string.o term.o var.o
