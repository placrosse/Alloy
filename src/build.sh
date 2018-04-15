#!/bin/bash

set -e

../utils/alloy-font-generator

CC=gcc
CFLAGS="$CFLAGS -Wall -Wextra -Werror -Wfatal-errors -g"
#CFLAGS="$CFLAGS -nostdlib -nostartfiles -nodefaultlibs -nostdinc"
CFLAGS="$CFLAGS -fomit-frame-pointer -fno-stack-protector -mno-red-zone"
CFLAGS="$CFLAGS -I ../include"

$CC $CFLAGS -c alloy.c
$CC $CFLAGS -c xterm.c
$CC $CFLAGS -c host-linux.c

LD=gcc
LDFLAGS=

$LD $LDFLAGS alloy.o xterm.o host-linux.o ../lib/liballoy.a -o alloy
